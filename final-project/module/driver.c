#include "core.h"
#include "driver_specs.h"

wait_queue_head_t wait_queue;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static int driver_port_usage = 0;
static char selected_letter = '\0';

static int __init hangman_device_driver_init(void);
static void __exit hangman_device_driver_exit(void);
static int hangman_device_driver_open(struct inode *, struct file *);
static int hangman_device_driver_release(struct inode *, struct file *);
static ssize_t hangman_device_driver_read(struct file *, char *, size_t,
                                          loff_t *);
static int hangman_device_driver_write(struct file *, const char __user *,
                                       size_t, loff_t *);
static long hangman_device_driver_ioctl(struct file *, unsigned int,
                                        unsigned long);

static struct file_operations device_driver_fops = {
    .owner = THIS_MODULE,
    .open = hangman_device_driver_open,
    .release = hangman_device_driver_release,
    .read = hangman_device_driver_read,
    .write = hangman_device_driver_write,
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
    initialize_timer();

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
    delete_timer_sync();
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
    selected_letter = '\0';

    register_interrupts();

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
    delete_timer_sync();

    return SUCCESS;
}

static ssize_t hangman_device_driver_read(struct file *inode, char *gdata,
                                          size_t length, loff_t *off_what) {
    if (copy_to_user(gdata, &selected_letter, length)) {
        return -EFAULT;
    }

    return length;
}

/*
 * Called on `write()`.
 */
static int hangman_device_driver_write(struct file *file,
                                       const char __user *buf, size_t count,
                                       loff_t *f_pos) {
    logger(INFO, "[hangman_device_driver] write\n");
    // sleep_app();

    return 0;
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
void set_selected_letter(const char letter) {
    selected_letter = letter;
    fpga_dot_write(selected_letter);
}

module_init(hangman_device_driver_init);
module_exit(hangman_device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
