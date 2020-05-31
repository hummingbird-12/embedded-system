#include "core.h"

static sw_state stopwatch_state;

static void timer_callback(unsigned long);
static void print_state(const sw_state*);
static void add_next_timer(void);

/*
 * Sets the initial state of the stopwatch
 */
void initialize_stopwatch(void) {
    logger(INFO, "[timer] initializing stopwatch\n");

    fpga_fnd_write(0, 0);

    stopwatch_state.elapsed_seconds = 0;
    stopwatch_state.pause_offset = 0;
}

/*
 * Starts the stopwatch.
 */
void start_stopwatch(void) {
    logger(INFO, "[timer] starting stopwatch\n");

    delete_timer();

    add_next_timer();
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
        get_jiffies_64() - stopwatch_state.last_callback;
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
static void add_next_timer(void) {
    const int offset = stopwatch_state.pause_offset;

    logger(INFO, "[timer] adding next timer with offset: %d\n", offset);

    stopwatch_state.timer.expires = get_jiffies_64() + HZ - offset;
    stopwatch_state.timer.data = (unsigned long) &stopwatch_state;
    stopwatch_state.timer.function = timer_callback;

    add_timer(&(stopwatch_state.timer));

    stopwatch_state.pause_offset = 0;
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

    payload->elapsed_seconds = (payload->elapsed_seconds + 1) % MAX_SECONDS;
    payload->last_callback = get_jiffies_64();

    // Print updated state
    print_state(payload);

    add_next_timer();
}

/*
 * Prints the state defined in `payload` in the FND device.
 */
static void print_state(const sw_state* payload) {
    const int seconds = payload->elapsed_seconds;
    fpga_fnd_write(seconds / 60, seconds % 60);
}
