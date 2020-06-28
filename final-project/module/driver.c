#include "core.h"
#include "driver_specs.h"

wait_queue_head_t wait_queue;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int driver_port_usage = 0;

static const char WORDS[WORDS_CNT][WORD_MAX_LEN] = {
    "EMBEDDED",        "ARM",          "PROTAGONIST",  "EPILOGUE",
    "LEGACY",          "LORE",         "TIME",         "ENVIRONMENT",
    "INTERVIEW",       "PLOT",         "ETYMOLOGY",    "ALIBI",
    "EVIDENCE",        "INFERENCE",    "INVESTIGATOR", "MYSTERY",
    "SLEUTH",          "SUSPECT",      "VICTIM",       "WITNESS",
    "IDIOM",           "IMAGERY",      "METER",        "METAPHOR",
    "SIMILE",          "ALLITERATION", "ONOMATOPOEIA", "MEMOIR",
    "PERSONIFICATION", "REALISM"};
static int word_index = -1;
static char current_guess[WORD_MAX_LEN];

static char selected_letter = '\0';
static char available_letters[26];

static int score = 0;
static int bonus_score = 1;
static int lives_left = 8;

static int __init hangman_device_driver_init(void);
static void __exit hangman_device_driver_exit(void);
static int hangman_device_driver_open(struct inode *, struct file *);
static int hangman_device_driver_release(struct inode *, struct file *);
static ssize_t hangman_device_driver_read(struct file *, char *, size_t,
                                          loff_t *);
static long hangman_device_driver_ioctl(struct file *, unsigned int,
                                        unsigned long);

static void game_start_next_word(void);

static struct file_operations device_driver_fops = {
    .owner = THIS_MODULE,
    .open = hangman_device_driver_open,
    .release = hangman_device_driver_release,
    .read = hangman_device_driver_read,
    .unlocked_ioctl = hangman_device_driver_ioctl,
};

/*
 * Called on `insmod`.
 * Registers the device driver, io-maps the FPGA devices
 * and initializes timers.
 */
static int __init hangman_device_driver_init(void) {
    const int registration =
        register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_driver_fops);

    logger(INFO, "[hangman_device_driver] init\n");

    if (registration != 0) {
        logger(ERROR, "[hangman_device_driver] Failed registering device.\n");
        return registration;
    }

    logger(INFO, "[hangman_device_driver] device file: /dev/%s\n", DEVICE_NAME);
    logger(INFO, "[hangman_device_driver] device major number: %d\n",
           DEVICE_MAJOR_NUMBER);

    fpga_iomap_devices();
    initialize_timers();

    return SUCCESS;
}

/*
 * Called on `rmmod`.
 * Deregisters the device driver, deletes timer sync
 * and io-unmaps the FPGA devices.
 */
static void __exit hangman_device_driver_exit(void) {
    logger(INFO, "[hangman_device_driver] exit\n");

    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    delete_timers_sync();
    fpga_iounmap_devices();
}

/*
 * Called on `open()`.
 * Keeps track of driver's usage and registers interrupts.
 */
static int hangman_device_driver_open(struct inode *inode, struct file *file) {
    logger(INFO, "[hangman_device_driver] open\n");

    if (driver_port_usage != 0) {
        return -EBUSY;
    }

    driver_port_usage = 1;
    register_interrupts();

    score = 0;
    word_index = -1;
    game_start_next_word();

    return SUCCESS;
}

/*
 * Called on `close()`.
 * Keeps track of driver's usage and releases interrupts.
 */
static int hangman_device_driver_release(struct inode *inode,
                                         struct file *file) {
    logger(INFO, "[hangman_device_driver] release\n");

    driver_port_usage = 0;

    release_interrupts();
    fpga_initialize();
    delete_timers_sync();

    return SUCCESS;
}

static ssize_t hangman_device_driver_read(struct file *inode, char *gdata,
                                          size_t length, loff_t *off_what) {
    struct _payload {
        char word[WORD_MAX_LEN];
        int status;
        int score;
    } payload;
    memset(payload.word, '\0', WORD_MAX_LEN);

    if (selected_letter == '\0') {
        payload.status = STATUS_EXIT;
    } else if (strcmp(WORDS[word_index], current_guess) == 0) {
        logger(INFO, "[hangman] %s is a correct guess!\n", WORDS[word_index]);

        strncpy(payload.word, WORDS[word_index], strlen(WORDS[word_index]));
        payload.status = STATUS_GUESSED;

        score += 10 * strlen(WORDS[word_index]) + (bonus_score != 0 ? 50 : 0);
        fpga_fnd_write(score);

        game_start_next_word();
    } else {
        strncpy(payload.word, current_guess, strlen(current_guess));
        payload.status = STATUS_INPUT;
    }
    payload.score = score;

    if (copy_to_user(gdata, &payload, length)) {
        return -EFAULT;
    }

    return length;
}

/*
 *
 */
static long hangman_device_driver_ioctl(struct file *file,
                                        unsigned int ioctl_num,
                                        unsigned long ioctl_param) {
    switch (ioctl_num) {
        case IOCTL_READ_LETTER:  // IOCTL to read chosen letter
            logger(INFO, "[hangman_device_driver] ioctl: IOCTL_READ_LETTER\n");

            start_switch_timer();
            sleep_app();

            break;
        default:
            logger(WARN,
                   "[hangman_device_driver] ioctl: Unrecognized ioctl command "
                   "%ud\n",
                   ioctl_num);
            return -1;
            break;
    }
    return SUCCESS;
}

/*
 * Puts app process into wait queue.
 */
void sleep_app(void) {
    logger(INFO, "[hangman_device_driver] Putting process into wait queue\n");

    interruptible_sleep_on(&wait_queue);
}

/*
 * Wakes up app process.
 */
void wake_app(void) {
    logger(INFO, "[hangman_device_driver] Waking up app process\n");

    __wake_up(&wait_queue, 1, 1, NULL);
}

/*
 *
 */
void game_set_selected_letter(const char letter) {
    selected_letter = letter;
    fpga_dot_write(selected_letter);
}

/*
 *
 */
void game_make_guess(void) {
    int i;
    int good_guess = 0;

    if (available_letters[selected_letter - 'A'] != 0) {
        for (i = 0; i < strlen(WORDS[word_index]); i++) {
            if (WORDS[word_index][i] == selected_letter) {
                current_guess[i] = selected_letter;
                good_guess = 1;
            }
        }
    }

    logger(INFO, "[hangman] attempted guess %c\n", selected_letter);
    logger(INFO, "[hangman] current guess %s for word %s\n", current_guess,
           WORDS[word_index]);

    if (good_guess == 0) {
        lives_left--;
        fpga_led_write(lives_left);

        if (lives_left <= 0) {
            game_exit();
            return;
        }
    }

    available_letters[selected_letter - 'A'] = 0;
    fpga_text_lcd_write(available_letters);

    delete_switch_timer();
    wake_app();
}

/*
 *
 */
void game_skip_word(void) {
    logger(INFO, "[hangman] skipping word: %s\n", WORDS[word_index]);

    game_start_next_word();
}

/*
 *
 */
void game_expire_bonus(void) {
    logger(INFO, "[hangman] bonus score time expired\n");
    bonus_score = 0;
}

/*
 *
 */
void game_exit(void) {
    logger(INFO, "[hangman] exit game\n");

    selected_letter = '\0';

    delete_timers();
    wake_app();
}

/*
 *
 */
static void game_start_next_word(void) {
    word_index = (word_index + 1) % WORDS_CNT;

    logger(INFO, "[hangman] new hangman word: %s\n", WORDS[word_index]);

    selected_letter = '\0';
    memset(available_letters, 1, 26);
    memset(current_guess, '\0', WORD_MAX_LEN);
    memset(current_guess, ' ', strlen(WORDS[word_index]));

    lives_left = MAX_LIVES;
    bonus_score = 1;
    delete_score_timer();
    start_score_timer(strlen(WORDS[word_index]) * BONUS_TIME_PER_CHAR);

    fpga_dot_write('\0');
    fpga_led_write(lives_left);
    fpga_text_lcd_write(available_letters);
}

module_init(hangman_device_driver_init);
module_exit(hangman_device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
