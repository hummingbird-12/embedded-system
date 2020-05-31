#include "core.h"

static sw_state stopwatch_state;
static struct timer_list exit_timer;

static void stopwatch_timer_callback(unsigned long);
static void add_next_stopwatch_timer(void);
static void print_stopwatch_state(const sw_state*);
static void exit_timer_callback(unsigned long);

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

    delete_stopwatch_timer();

    add_next_stopwatch_timer();
}

/*
 * Ends the stopwatch.
 */
void end_stopwatch(void) {
    logger(INFO, "[timer] ending stopwatch\n");

    delete_stopwatch_timer();
    fpga_fnd_write(0, 0);

    wake_app();
}

/*
 * Pauses the stopwatch.
 */
void pause_stopwatch(void) {
    logger(INFO, "[timer] pausing stopwatch\n");

    delete_stopwatch_timer();
    stopwatch_state.pause_offset =
        get_jiffies_64() - stopwatch_state.last_callback;
}

/*
 * Resets the stopwatch.
 */
void reset_stopwatch(void) {
    logger(INFO, "[timer] resetting stopwatch\n");

    delete_stopwatch_timer();
    initialize_stopwatch();
}

/*
 * Registers next stopwatch timer.
 */
static void add_next_stopwatch_timer(void) {
    const int offset = stopwatch_state.pause_offset;

    logger(INFO, "[timer] adding next timer with offset: %d\n", offset);

    stopwatch_state.timer.expires = get_jiffies_64() + HZ - offset;
    stopwatch_state.timer.data = (unsigned long) &stopwatch_state;
    stopwatch_state.timer.function = stopwatch_timer_callback;

    add_timer(&(stopwatch_state.timer));

    stopwatch_state.pause_offset = 0;
}

/*
 * The callback function that is called
 * every time the stopwatch timer expires.
 */
static void stopwatch_timer_callback(unsigned long timeout) {
    sw_state* payload = (sw_state*) timeout;

    payload->elapsed_seconds = (payload->elapsed_seconds + 1) % MAX_SECONDS;
    payload->last_callback = get_jiffies_64();

    // Print updated state
    print_stopwatch_state(payload);

    add_next_stopwatch_timer();
}

/*
 * Prints the state defined in `payload` in the FND device.
 */
static void print_stopwatch_state(const sw_state* payload) {
    const int seconds = payload->elapsed_seconds;
    fpga_fnd_write(seconds / 60, seconds % 60);
}

/*
 * Deletes the stopwatch timer.
 * For interrupt context.
 */
void delete_stopwatch_timer(void) { del_timer(&(stopwatch_state.timer)); }

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
static void exit_timer_callback(unsigned long timeout) {
    logger(INFO, "[timer] exit timer expired\n");

    end_stopwatch();
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
 * Initializes the stopwatch and exit timers.
 */
void initizlize_timers(void) {
    init_timer(&(stopwatch_state.timer));
    init_timer(&exit_timer);
}

/*
 * Deletes the stopwatch and exit timers' sync.
 * For non-interrupt context.
 */
void delete_timers_sync(void) {
    del_timer_sync(&(stopwatch_state.timer));
    del_timer_sync(&exit_timer);
}
