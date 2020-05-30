#include "core.h"

wait_queue_head_t wait_queue;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static irqreturn_t home_btn_handler(int, void*);
static irqreturn_t back_btn_handler(int, void*);
static irqreturn_t vol_up_btn_handler(int, void*);
static irqreturn_t vol_down_btn_handler(int, void*);

void register_interrupts(void) {
    int irq, ret;

    // Register HOME button interrupt
    gpio_direction_input(GPIO_HOME);
    irq = gpio_to_irq(GPIO_HOME);
    logger(INFO, "Home button IRQ Number : %d\n", irq);
    ret =
        request_irq(irq, home_btn_handler, IRQF_TRIGGER_RISING, "Home btn", 0);

    // Register BACK button interrupt
    gpio_direction_input(GPIO_BACK);
    irq = gpio_to_irq(GPIO_BACK);
    logger(INFO, "Back button IRQ Number : %d\n", irq);
    ret =
        request_irq(irq, back_btn_handler, IRQF_TRIGGER_RISING, "Back btn", 0);

    // Register VOL+ button interrupt
    gpio_direction_input(GPIO_VOLUP);
    irq = gpio_to_irq(GPIO_VOLUP);
    logger(INFO, "Vol+ button IRQ Number : %d\n", irq);
    ret = request_irq(irq, vol_up_btn_handler, IRQF_TRIGGER_RISING, "Vol+ btn",
                      0);

    // Register VOL- button interrupt
    gpio_direction_input(GPIO_VOLDOWN);
    irq = gpio_to_irq(GPIO_VOLDOWN);
    logger(INFO, "Vol- button IRQ Number : %d\n", irq);
    ret = request_irq(irq, vol_down_btn_handler, IRQF_TRIGGER_RISING,
                      "Vol- btn", 0);
}

void release_interrupts(void) {
    logger(INFO, "Releasing interrupt handlers\n");

    free_irq(gpio_to_irq(GPIO_HOME), NULL);
    free_irq(gpio_to_irq(GPIO_BACK), NULL);
    free_irq(gpio_to_irq(GPIO_VOLUP), NULL);
    free_irq(gpio_to_irq(GPIO_VOLDOWN), NULL);
}

void sleep_app(void) {
    logger(INFO, "Putting process into wait queue\n");

    interruptible_sleep_on(&wait_queue);
}

static irqreturn_t home_btn_handler(int irq, void* dev_id) {
    logger(INFO, "Handling interrupt by Home button\n");
    start_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t back_btn_handler(int irq, void* dev_id) {
    logger(INFO, "Handling interrupt by Back button\n");
    pause_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_up_btn_handler(int irq, void* dev_id) {
    logger(INFO, "Handling interrupt by VOL+ button\n");
    reset_stopwatch();

    return IRQ_HANDLED;
}

static irqreturn_t vol_down_btn_handler(int irq, void* dev_id) {
    logger(INFO, "Handling interrupt by VOL- button\n");
    end_stopwatch();

    logger(INFO, "Waking up process\n");
    __wake_up(&wait_queue, 1, 1, NULL);
    return IRQ_HANDLED;
}
