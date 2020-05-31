#include "core.h"

wait_queue_head_t wait_queue;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static irqreturn_t home_btn_handler(int, void*);
static irqreturn_t back_btn_handler(int, void*);
static irqreturn_t vol_up_btn_handler(int, void*);
static irqreturn_t vol_down_btn_handler(int, void*);

static enum _BTN_PRESS {
    INIT,
    HOME,
    BACK,
    VOL_UP,
    VOL_DOWN,
} last_pressed = INIT;

static int is_vol_down_pressing = 0;

void register_interrupts(void) {
    int irq, ret;

    // Register HOME button interrupt
    gpio_direction_input(GPIO_HOME);
    irq = gpio_to_irq(GPIO_HOME);
    logger(INFO, "[interrupt] Home button IRQ Number : %d\n", irq);
    ret = request_irq(irq, home_btn_handler, IRQF_TRIGGER_RISING, NAME_HOME, 0);

    // Register BACK button interrupt
    gpio_direction_input(GPIO_BACK);
    irq = gpio_to_irq(GPIO_BACK);
    logger(INFO, "[interrupt] Back button IRQ Number : %d\n", irq);
    ret = request_irq(irq, back_btn_handler, IRQF_TRIGGER_RISING, NAME_BACK, 0);

    // Register VOL+ button interrupt
    gpio_direction_input(GPIO_VOLUP);
    irq = gpio_to_irq(GPIO_VOLUP);
    logger(INFO, "[interrupt] Vol+ button IRQ Number : %d\n", irq);
    ret = request_irq(irq, vol_up_btn_handler, IRQF_TRIGGER_RISING, NAME_VOLUP,
                      0);

    // Register VOL- button interrupt
    gpio_direction_input(GPIO_VOLDOWN);
    irq = gpio_to_irq(GPIO_VOLDOWN);
    logger(INFO, "[interrupt] Vol- button IRQ Number : %d\n", irq);
    ret = request_irq(irq, vol_down_btn_handler,
                      IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, NAME_VOLDOWN,
                      0);

    last_pressed = INIT;
    is_vol_down_pressing = 0;
}

void release_interrupts(void) {
    logger(INFO, "[interrupt] Releasing interrupt handlers\n");

    free_irq(gpio_to_irq(GPIO_HOME), NULL);
    free_irq(gpio_to_irq(GPIO_BACK), NULL);
    free_irq(gpio_to_irq(GPIO_VOLUP), NULL);
    free_irq(gpio_to_irq(GPIO_VOLDOWN), NULL);
}

void sleep_app(void) {
    logger(INFO, "[interrupt] Putting process into wait queue\n");

    interruptible_sleep_on(&wait_queue);
}

void wake_app(void) {
    logger(INFO, "[interrupt] Waking up app process\n");

    __wake_up(&wait_queue, 1, 1, NULL);
}

static irqreturn_t home_btn_handler(int irq, void* dev_id) {
    if (last_pressed == HOME) {
        return IRQ_HANDLED;
    }
    last_pressed = HOME;

    logger(INFO, "[interrupt] Handling interrupt by Home button release\n");
    start_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t back_btn_handler(int irq, void* dev_id) {
    if (last_pressed == BACK) {
        return IRQ_HANDLED;
    }
    if (last_pressed != HOME) {
        logger(INFO,
               "[interrupt] Ignoring Back button interrupt when stopwatch is "
               "not running\n");
        return IRQ_HANDLED;
    }
    last_pressed = BACK;

    logger(INFO, "[interrupt] Handling interrupt by Back button release\n");
    pause_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_up_btn_handler(int irq, void* dev_id) {
    if (last_pressed == VOL_UP) {
        return IRQ_HANDLED;
    }
    last_pressed = VOL_UP;

    logger(INFO, "[interrupt] Handling interrupt by VOL+ button release\n");
    reset_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_down_btn_handler(int irq, void* dev_id) {
    // Vol- is pressed
    if (is_vol_down_pressing == 0) {
        logger(INFO, "[interrupt] Handling interrupt by VOL- button press\n");
        start_exit_timer();
    }
    // Vol- is released
    else {
        logger(INFO, "[interrupt] Handling interrupt by VOL- button release\n");
        delete_exit_timer();
        last_pressed = VOL_DOWN;
    }

    is_vol_down_pressing = 1 - is_vol_down_pressing;

    return IRQ_HANDLED;
}
