#include "core.h"

static t_state timer_state;

static void timer_callback(unsigned long);
static void print_state(const t_state*);
static void move_text(t_state*, const int);

/*
 * Sets the initial state as defined by the parameters.
 */
void initialize_timer(const char* init, const int interval, const int count) {
    int i;

    logger(INFO,
           "[timer_initialize] Received parameters: init = %s\tinterval = "
           "%d\tcount = %d\n",
           init, interval, count);

    // Set state
    // See `timer.h` for each data's meaning
    for (i = 0; i < 4; i++) {
        if (init[i] != '0') {
            timer_state.digit = init[i] - '0';
            timer_state.digit_index = i;
            break;
        }
    }
    timer_state.text_index[0] = timer_state.text_index[1] = 0;
    timer_state.text_direction[0] = timer_state.text_direction[1] = RIGHT;
    strncpy(timer_state.text[0], STUDENT_ID, strlen(STUDENT_ID));
    strncpy(timer_state.text[1], STUDENT_NAME, strlen(STUDENT_NAME));

    timer_state.count = 0;
    timer_state.COUNT_END = count;
    timer_state.INTERVAL = interval;

    // Initialize timer
    init_timer(&(timer_state.timer));
}

/*
 * Starts the timer feature.
 */
void start_timer(void) {
    del_timer_sync(&(timer_state.timer));

    print_state(&timer_state);

    // Register next timer
    timer_state.timer.expires =
        get_jiffies_64() + timer_state.INTERVAL * (HZ / 10);
    timer_state.timer.data = (unsigned long) &timer_state;
    timer_state.timer.function = timer_callback;

    add_timer(&(timer_state.timer));
}

/*
 * Deletes timer's sync.
 */
void delete_timer(void) { del_timer_sync(&(timer_state.timer)); }

/*
 * The callback function that is called
 * every time the timer expires.
 */
static void timer_callback(unsigned long timeout) {
    t_state* payload = (t_state*) timeout;

    // If count has reached the total number of counts
    if (++(payload->count) == payload->COUNT_END) {
        // Clear out digit and text state
        payload->digit = 0;
        payload->text[0][0] = payload->text[1][0] = '\0';
    }
    // If count has NOT YET reached the total number of counts
    else {
        // Update digit state
        payload->digit =
            (payload->digit == STATE_ROTATION ? 1 : (payload->digit + 1));
        if (payload->count % STATE_ROTATION == 0) {
            payload->digit_index = (payload->digit_index + 1) % 4;
        }

        // Update text state
        move_text(payload, 0);
        move_text(payload, 1);

        // Register next timer
        payload->timer.expires =
            get_jiffies_64() + payload->INTERVAL * (HZ / 10);
        payload->timer.data = (unsigned long) &timer_state;
        payload->timer.function = timer_callback;
        add_timer(&(payload->timer));
    }

    // Print updated state
    print_state(payload);
}

/*
 * Prints the state defined in `payload` in the FPGA devices.
 */
static void print_state(const t_state* payload) {
    logger(INFO, "Current timer count: %d\n", payload->count);

    fpga_dot_write(payload->digit);
    fpga_fnd_write(payload->digit_index, payload->digit);
    fpga_led_write(payload->digit);
    fpga_text_lcd_write(payload->text[0], payload->text_index[0],
                        payload->text[1], payload->text_index[1]);
}

/*
 * Updates the text state.
 * `line` is either 0 or 1, meaning first or second line respectively.
 */
static void move_text(t_state* payload, const int line) {
    if (payload->text_direction[line] == RIGHT) {
        if (++(payload->text_index[line]) + strlen(payload->text[line]) >
            TEXT_LCD_BUFFER_SIZE / 2) {
            payload->text_index[line] -= 2;
            payload->text_direction[line] = LEFT;
        }
    } else {
        if (--(payload->text_index[line]) < 0) {
            payload->text_index[line] += 2;
            payload->text_direction[line] = RIGHT;
        }
    }
}
