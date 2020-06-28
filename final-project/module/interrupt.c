#include "core.h"

static irqreturn_t home_btn_handler(int, void*);
static irqreturn_t back_btn_handler(int, void*);
static irqreturn_t vol_up_btn_handler(int, void*);
static irqreturn_t vol_down_btn_handler(int, void*);

// static enum _BTN_PRESS {
//     INIT,
//     HOME,
//     BACK,
//     VOL_UP,
//     VOL_DOWN,
// } last_pressed =
//     INIT;  // Pressed button tracker to prevent consecutive pressing

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

    // last_pressed = INIT;
    is_vol_down_pressing = 0;
}

/*
 * Releases interrupts.
 */
void release_interrupts(void) {
    logger(INFO, "[interrupt] Releasing interrupt handlers\n");

    free_irq(gpio_to_irq(GPIO_HOME), NULL);
    free_irq(gpio_to_irq(GPIO_BACK), NULL);
    free_irq(gpio_to_irq(GPIO_VOLUP), NULL);
    free_irq(gpio_to_irq(GPIO_VOLDOWN), NULL);
}

/*
 * Interrupt handler for Home button.
 */
static irqreturn_t home_btn_handler(int irq, void* dev_id) {
    // Ignore consecutive pressing
    // if (last_pressed == HOME) {
    //     return IRQ_HANDLED;
    // }
    // last_pressed = HOME;

    logger(INFO, "[interrupt] Handling interrupt by Home button release\n");
    game_make_guess();

    return IRQ_HANDLED;
}

/*
 * Interrupt handler for Back button.
 */
static irqreturn_t back_btn_handler(int irq, void* dev_id) {
    // Ignore consecutive pressing
    // if (last_pressed == BACK) {
    //     return IRQ_HANDLED;
    // }
    // last_pressed = BACK;

    logger(INFO, "[interrupt] Handling interrupt by Back button release\n");

    return IRQ_HANDLED;
}

/*
 * Interrupt handler for Vol+ button.
 */
static irqreturn_t vol_up_btn_handler(int irq, void* dev_id) {
    // Ignore consecutive pressing
    // if (last_pressed == VOL_UP) {
    //     return IRQ_HANDLED;
    // }
    // last_pressed = VOL_UP;

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
        // start_exit_timer();
    }
    // Vol- is released
    else {
        logger(INFO, "[interrupt] Handling interrupt by VOL- button release\n");
        // delete_exit_timer();
        // last_pressed = VOL_DOWN;
    }

    is_vol_down_pressing = 1 - is_vol_down_pressing;

    return IRQ_HANDLED;
}
