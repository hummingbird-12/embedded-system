#include "core.h"

static sw_state stopwatch_state;

static void timer_callback(unsigned long);
static void print_state(const sw_state*);
static void add_next_timer(const int);

/*
 * Sets the initial state of the stopwatch
 */
void initialize_stopwatch(void) {
    logger(INFO, "[timer] initializing stopwatch\n");

    fpga_fnd_write(0, 0);

    stopwatch_state.pause_offset = 0;
}

/*
 * Starts the stopwatch.
 */
void start_stopwatch(void) {
    logger(INFO, "[timer] starting stopwatch\n");

    delete_timer();

    if (stopwatch_state.pause_offset == 0) {
        stopwatch_state.start_time = get_jiffies_64();
    }

    add_next_timer(stopwatch_state.pause_offset);
}

/*
 * Ends the stopwatch.
 */
void end_stopwatch(void) {
    logger(INFO, "[timer] ending stopwatch\n");

    delete_timer();
    fpga_fnd_write(0, 0);
}

/*
 * Pauses the stopwatch.
 */
void pause_stopwatch(void) {
    logger(INFO, "[timer] pausing stopwatch\n");

    delete_timer();
    stopwatch_state.pause_offset =
        (int) (get_jiffies_64() - stopwatch_state.start_time) % HZ;
}

/*
 * Resets the stopwatch.
 */
void reset_stopwatch(void) {
    logger(INFO, "[timer] resetting stopwatch\n");

    delete_timer();
    initialize_stopwatch();
}

/*
 * Initializes timer.
 */
void initizlize_timer(void) { init_timer(&(stopwatch_state.timer)); }

/*
 * Registers next timer.
 */
static void add_next_timer(const int offset) {
    stopwatch_state.timer.expires = get_jiffies_64() + offset * HZ;
    stopwatch_state.timer.data = (unsigned long) &stopwatch_state;
    stopwatch_state.timer.function = timer_callback;

    add_timer(&(stopwatch_state.timer));
}

/*
 * Deletes timer.
 * For interrupt context.
 */
void delete_timer(void) { del_timer(&(stopwatch_state.timer)); }

/*
 * Deletes timer's sync.
 * For non-interrupt context.
 */
void delete_timer_sync(void) { del_timer_sync(&(stopwatch_state.timer)); }

/*
 * The callback function that is called
 * every time the timer expires.
 */
static void timer_callback(unsigned long timeout) {
    sw_state* payload = (sw_state*) timeout;

    // Print updated state
    print_state(payload);

    add_next_timer(1);
}

/*
 * Prints the state defined in `payload` in the FND device.
 */
static void print_state(const sw_state* payload) {
    const int cur_time = (int) (get_jiffies_64() - payload->start_time) / HZ;
    fpga_fnd_write(cur_time / 60, cur_time % 60);
}
