#include "core.h"

static const char switch_letter[][3] = {
    {'A', 'B', 'C'}, {'D', 'E', 'F'}, {'G', 'H', 'I'},
    {'J', 'K', 'L'}, {'M', 'N', 'O'}, {'P', 'Q', 'R'},
    {'S', 'T', 'U'}, {'V', 'W', 'X'}, {'Y', 'Z', ' '},
};

static struct timer_list switch_timer;
static int last_switch = -1;
static int switch_index = 0;

static void add_next_switch_timer(void);
static void switch_timer_callback(unsigned long);

/*
 *
 */
void start_switch_timer(void) {
    logger(INFO, "[timer] starting switch timer\n");

    // last_switch = -1;
    // switch_index = 0;

    add_next_switch_timer();
}

/*
 * Registers next switch timer.
 */
static void add_next_switch_timer(void) {
    switch_timer.expires = get_jiffies_64() + HZ / 7;
    switch_timer.function = switch_timer_callback;

    add_timer(&switch_timer);
}

/*
 *
 */
static void switch_timer_callback(unsigned long data) {
    const int pressed_switch = fpga_switch_read();

    if (pressed_switch != -1) {
        if (pressed_switch == last_switch) {
            switch_index = (switch_index + 1) % (last_switch == 8 ? 2 : 3);
        } else {
            last_switch = pressed_switch;
            switch_index = 0;
        }
        logger(INFO, "[timer] selected letter: %c\n",
               switch_letter[last_switch][switch_index]);
        set_selected_letter(switch_letter[last_switch][switch_index]);
    } else {
        add_next_switch_timer();
    }
}

/*
 * Initializes the switch timer.
 */
void initialize_timer(void) {
    logger(INFO, "[timer] initializing switch timer\n");
    init_timer(&switch_timer);
}

/*
 * Deletes the switch timer.
 * For non-interrupt context.
 */
void delete_timer_sync(void) {
    logger(INFO, "[timer] deleting switch timer\n");
    del_timer_sync(&switch_timer);
}
