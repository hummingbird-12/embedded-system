#include "core.h"

static const char switch_letter[][3] = {
    {'A', 'B', 'C'}, {'D', 'E', 'F'}, {'G', 'H', 'I'},
    {'J', 'K', 'L'}, {'M', 'N', 'O'}, {'P', 'Q', 'R'},
    {'S', 'T', 'U'}, {'V', 'W', 'X'}, {'Y', 'Z', ' '},
};

static struct timer_list switch_timer;
static struct timer_list score_timer;
static struct timer_list exit_timer;

static int last_switch = -1;
static int switch_index = 0;

static void add_next_switch_timer(void);
static void switch_timer_callback(unsigned long);
static void score_timer_callback(unsigned long);
static void exit_timer_callback(unsigned long);

/*
 *
 */
void start_switch_timer(void) {
    logger(INFO, "[timer] starting switch timer\n");

    last_switch = -1;
    switch_index = 0;
    fpga_dot_write('\0');

    add_next_switch_timer();
}

/*
 * Deletes the switch timer.
 * For interrupt context.
 */
void delete_switch_timer(void) {
    logger(INFO, "[timer] deleting switch timer\n");

    del_timer(&switch_timer);
}

/*
 * Registers next switch timer.
 */
static void add_next_switch_timer(void) {
    switch_timer.expires = get_jiffies_64() + HZ / 7;
    switch_timer.function = switch_timer_callback;

    add_timer(&switch_timer);
    enable_input();
}

/*
 *
 */
static void switch_timer_callback(unsigned long data) {
    const int pressed_switch = fpga_switch_read();
    disable_input();

    if (pressed_switch != -1) {
        if (pressed_switch == last_switch) {
            switch_index = (switch_index + 1) % (last_switch == 8 ? 2 : 3);
        } else {
            last_switch = pressed_switch;
            switch_index = 0;
        }
        logger(INFO, "[timer] selected letter: %c\n",
               switch_letter[last_switch][switch_index]);
        game_set_selected_letter(switch_letter[last_switch][switch_index]);
    }
    add_next_switch_timer();
}

/*
 *
 */
void start_score_timer(const int seconds) {
    logger(INFO, "[timer] starting score timer for %d seconds\n", seconds);

    score_timer.expires = get_jiffies_64() + HZ * seconds;
    score_timer.function = score_timer_callback;

    add_timer(&score_timer);
}

/*
 *
 */
void delete_score_timer(void) {
    logger(INFO, "[timer] deleting score timer\n");
    del_timer_sync(&score_timer);
}

/*
 *
 */
static void score_timer_callback(unsigned long data) { game_expire_bonus(); }

/*
 * Starts the exit timer.
 */
void start_exit_timer(void) {
    delete_exit_timer();

    logger(INFO, "[timer] adding exit timer\n");

    exit_timer.expires = get_jiffies_64() + EXIT_TIMEOUT_SECS * HZ;
    exit_timer.function = exit_timer_callback;

    add_timer(&exit_timer);
}

/*
 * The callback function that is called
 * once the exit timer expires.
 */
static void exit_timer_callback(unsigned long data) {
    logger(INFO, "[timer] exit timer expired\n");

    game_exit();
}

/*
 * Deletes the exit timer.
 * For interrupt context.
 */
void delete_exit_timer(void) {
    logger(INFO, "[timer] deleting any previous exit timer\n");

    del_timer(&exit_timer);
}

/*
 * Initializes the timers.
 */
void initialize_timers(void) {
    logger(INFO, "[timer] initializing timers\n");
    init_timer(&switch_timer);
    init_timer(&score_timer);
    init_timer(&exit_timer);
}

/*
 * Deletes the timers.
 * For non-interrupt context.
 */
void delete_timers_sync(void) {
    logger(INFO, "[timer] deleting timers\n");
    del_timer_sync(&switch_timer);
    del_timer_sync(&score_timer);
    del_timer_sync(&exit_timer);
}

/*
 * Deletes the timers.
 * For interrupt context.
 */
void delete_timers(void) {
    logger(INFO, "[timer] deleting timers (sync)\n");
    del_timer(&switch_timer);
    del_timer(&score_timer);
    del_timer(&exit_timer);
}
