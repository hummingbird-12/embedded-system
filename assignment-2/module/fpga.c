#include "core.h"
#include "fpga_dot.h"

char* fpga_addr[FPGA_DEVICES_CNT];

static const char* FPGA_DEVICES_STR[] = {"DOT", "FND", "LED", "TEXT_LCD"};
static const int FPGA_HW_ADDR[] = {DOT_ADDRESS, FND_ADDRESS, LED_ADDRESS,
                                   TEXT_LCD_ADDRESS};

void fpga_iomap_devices(void) {
    fpga dev;

    logger(INFO, "IO-mapping FPGA devices\n");

    fpga_addr[DOT] = ioremap(FPGA_HW_ADDR[DOT], 0xA);
    fpga_addr[FND] = ioremap(FPGA_HW_ADDR[FND], 0x2);
    fpga_addr[LED] = ioremap(FPGA_HW_ADDR[LED], 0x1);
    fpga_addr[TEXT_LCD] = ioremap(FPGA_HW_ADDR[TEXT_LCD], 0x32);

    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        if (fpga_addr[dev] == NULL) {
            logger(ERROR, "Failed to IO-map device %s\n",
                   FPGA_DEVICES_STR[dev]);
        }
    }
}

void fpga_iounmap_devices(void) {
    fpga dev;

    logger(INFO, "IO-unmapping FPGA devices\n");

    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        iounmap(fpga_addr[dev]);
    }
}

/*
 * Prints given `digit` in FPGA's Dot Matrix device.
 * When `digit` is 0, the whole matrix is turned off.
 */
void fpga_dot_write(const int digit) {
    int i;

    logger(INFO, "Printing %d into Dot Matrix device\n", digit);

    for (i = 0; i < 10; i++) {
        outw(fpga_dot_digit[digit][i] & 0x7F,
             (unsigned int) fpga_addr[DOT] + i * 2);
    }
}

/*
 * Prints given `digit` at given `index` in FPGA's FND device.
 * `index` should be between 0 and 3, including.
 * `digit` should be between 0 and 8, including.
 * The digits of other indexes will be 0.
 */
void fpga_fnd_write(const int index, const int digit) {
    const unsigned short value = digit << (4 * index);

    logger(INFO, "Printing digit %d at index %d into FND device\n", digit,
           index);

    outw(value, (unsigned int) fpga_addr[FND]);
}

/*
 * Prints given `digit` in FPGA's FND device.
 * `digit` should be between 0 and 8, including.
 * When `digit` is 0, all LEDs will be turned off.
 */
void fpga_led_write(const int digit) {
    const unsigned short value = ((digit > 0) ? (1 << (8 - digit)) : 0);

    logger(INFO, "Turning on LED %d of LED device\n", digit);

    outw(value, (unsigned int) fpga_addr[LED]);
}
