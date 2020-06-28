#include "core.h"

static irqreturn_t home_btn_handler(int, void*);
static irqreturn_t vol_up_btn_handler(int, void*);
static irqreturn_t vol_down_btn_handler(int, void*);

static int input_enabled = 0;
static int is_vol_down_pressing = 0;  // Flag showing Vol- button's status

/*
 * Interrupt registration and initialization.
 */
void register_interrupts(void) {
    int irq, ret;

    // Register HOME button interrupt
    gpio_direction_input(GPIO_HOME);
    irq = gpio_to_irq(GPIO_HOME);
    logger(INFO, "[interrupt] Home button IRQ Number : %d\n", irq);
    ret = request_irq(irq, home_btn_handler, IRQF_TRIGGER_RISING, NAME_HOME, 0);

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

    is_vol_down_pressing = 0;
    input_enabled = 0;
}

/*
 * Releases interrupts.
 */
void release_interrupts(void) {
    logger(INFO, "[interrupt] Releasing interrupt handlers\n");

    free_irq(gpio_to_irq(GPIO_HOME), NULL);
    free_irq(gpio_to_irq(GPIO_VOLUP), NULL);
    free_irq(gpio_to_irq(GPIO_VOLDOWN), NULL);
}

/*
 * Interrupt handler for Home button.
 */
static irqreturn_t home_btn_handler(int irq, void* dev_id) {
    if (input_enabled == 0) {
        return IRQ_HANDLED;
    }

    logger(INFO, "[interrupt] Handling interrupt by Home button release\n");
    game_make_guess();

    return IRQ_HANDLED;
}

/*
 * Interrupt handler for Vol+ button.
 */
static irqreturn_t vol_up_btn_handler(int irq, void* dev_id) {
    logger(INFO, "[interrupt] Handling interrupt by VOL+ button release\n");

    game_skip_word();

    return IRQ_HANDLED;
}

/*
 * Interrupt handler for Vol- button.
 */
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
    }

    is_vol_down_pressing = 1 - is_vol_down_pressing;

    return IRQ_HANDLED;
}

/*
 * Accept interrupts.
 */
void enable_input(void) { input_enabled = 1; }

/*
 * Reject interrupts.
 * Avoids double submissions.
 */
void disable_input(void) { input_enabled = 0; }
