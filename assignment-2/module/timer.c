#include "core.h"

static t_state timer_state;

void timer_initialize(const char* init, const int interval, const int count) {
    int i;
    for (i = 0; i < 4; i++) {
        if (init[i] != '0') {
            timer_state.digit = init[i] - '0';
            timer_state.digit_index = i;
            break;
        }
    }
    timer_state.text_index[0] = timer_state.text_index[1] = 0;
    timer_state.text_direction[0] = timer_state.text_direction[1] = RIGHT;
    strncpy(timer_state.text[0], STUDENT_ID, 8);
    strncpy(timer_state.text[1], STUDENT_NAME, 8);

    timer_state.COUNT_END = count;
    timer_state.INTERVAL = interval;

    init_timer(&(timer_state.timer));
}

void timer_start(void) {
    print_state(&timer_state);

    timer_state.timer.expires =
        get_jiffies_64() + timer_state.INTERVAL * (HZ / 10);
    timer_state.timer.data = (unsigned long) &timer_state;
    timer_state.timer.function = timer_callback;

    add_timer(&(timer_state.timer));
}

void timer_callback(unsigned long timeout) {
    t_state* payload = (t_state*) timeout;

    if (++(payload->count) == payload->COUNT_END) {
        payload->digit = 0;
        payload->text[0][0] = payload->text[1][0] = '\0';
    } else {
        payload->digit = (payload->digit + 1) % STATE_ROTATION;
        if (++(payload->count) % STATE_ROTATION == 0) {
            payload->digit_index = (payload->digit_index + 1) % STATE_ROTATION;
        }

        move_text(payload, 0);
        move_text(payload, 1);
    }
    print_state(payload);

    payload->timer.expires = get_jiffies_64() + payload->INTERVAL * HZ;
    payload->timer.data = (unsigned long) &timer_state;
    payload->timer.function = timer_callback;
    add_timer(&(payload->timer));
}

void print_state(const t_state* payload) {
    fpga_dot_write(payload->digit);
    fpga_fnd_write(payload->digit_index, payload->digit);
    fpga_led_write(payload->digit);
    fpga_text_lcd_write(STUDENT_ID, payload->text_index[0], STUDENT_NAME,
                        payload->text_index[1]);
}

void move_text(t_state* payload, const int line) {
    if (payload->text_direction[line] == RIGHT) {
        if (++(payload->text_index[line]) + strlen(payload->text[line]) >
            TEXT_LCD_BUFFER_SIZE / 2) {
            --(payload->text_index[line]);
            payload->text_direction[line] = LEFT;
        }
    } else {
        if (--(payload->text_index[line]) < 0) {
            ++(payload->text_index[line]);
            payload->text_direction[line] = RIGHT;
        }
    }
}
