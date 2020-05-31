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

void register_interrupts(void) {
    int irq, ret;

    // Register HOME button interrupt
    gpio_direction_input(GPIO_HOME);
    irq = gpio_to_irq(GPIO_HOME);
    logger(INFO, "[interrupt] Home button IRQ Number : %d\n", irq);
    ret = request_irq(irq, home_btn_handler, IRQF_TRIGGER_RISING, "home", 0);

    // Register BACK button interrupt
    gpio_direction_input(GPIO_BACK);
    irq = gpio_to_irq(GPIO_BACK);
    logger(INFO, "[interrupt] Back button IRQ Number : %d\n", irq);
    ret = request_irq(irq, back_btn_handler, IRQF_TRIGGER_RISING, "back", 0);

    // Register VOL+ button interrupt
    gpio_direction_input(GPIO_VOLUP);
    irq = gpio_to_irq(GPIO_VOLUP);
    logger(INFO, "[interrupt] Vol+ button IRQ Number : %d\n", irq);
    ret = request_irq(irq, vol_up_btn_handler, IRQF_TRIGGER_RISING, "vol+", 0);

    // Register VOL- button interrupt
    gpio_direction_input(GPIO_VOLDOWN);
    irq = gpio_to_irq(GPIO_VOLDOWN);
    logger(INFO, "[interrupt] Vol- button IRQ Number : %d\n", irq);
    ret =
        request_irq(irq, vol_down_btn_handler, IRQF_TRIGGER_RISING, "vol-", 0);
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

static irqreturn_t home_btn_handler(int irq, void* dev_id) {
    if (last_pressed == HOME) {
        return IRQ_HANDLED;
    }
    last_pressed = HOME;

    logger(INFO, "[interrupt] Handling interrupt by Home button\n");
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

    logger(INFO, "[interrupt] Handling interrupt by Back button\n");
    pause_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_up_btn_handler(int irq, void* dev_id) {
    if (last_pressed == VOL_UP) {
        return IRQ_HANDLED;
    }
    last_pressed = VOL_UP;

    logger(INFO, "[interrupt] Handling interrupt by VOL+ button\n");
    reset_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_down_btn_handler(int irq, void* dev_id) {
    if (last_pressed == VOL_DOWN) {
        return IRQ_HANDLED;
    }
    last_pressed = VOL_DOWN;

    logger(INFO, "[interrupt] Handling interrupt by VOL- button\n");
    end_stopwatch();

    logger(INFO, "[interrupt] Waking up process\n");
    __wake_up(&wait_queue, 1, 1, NULL);
    return IRQ_HANDLED;
}
