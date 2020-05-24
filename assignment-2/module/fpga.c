#include "core.h"
#include "fpga_dot.h"

char* fpga_addr[FPGA_DEVICES_CNT];

static const char* FPGA_DEVICES_STR[] = {"DOT", "FND", "LED", "TEXT_LCD"};
static const int FPGA_HW_ADDR[] = {DOT_ADDRESS, FND_ADDRESS, LED_ADDRESS,
                                   TEXT_LCD_ADDRESS};

/*
 * IO-maps the FPGA's devices.
 */
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

/*
 * IO-unmaps the FPGA's devices.
 */
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
    const unsigned short value = digit << (12 - 4 * index);

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

/*
 * Prints given
 * `str1` starting at `start1` of the first line and
 * `str2` starting at `start2` of the second line
 * in FPGA's Text LCD device.
 * The lengths of `str1` and `str2` should be between 0 and 16, including.
 * `start1` and `start2` should be between 0 and 15, including.
 */
void fpga_text_lcd_write(const char* str1, const int start1, const char* str2,
                         const int start2) {
    int i;
    unsigned short int value;
    const int index1 = start1;
    const int index2 = TEXT_LCD_BUFFER_SIZE / 2 + start2;
    const int len1 = strlen(str1);
    const int len2 = strlen(str2);
    unsigned char buffer[TEXT_LCD_BUFFER_SIZE + 1] = {0};

    memset(buffer, ' ', index1);
    strncat(buffer, str1, len1);

    logger(INFO, "Writing line 1 \"%-16s\" into Text LCD device\n", buffer);

    memset(buffer + index1 + len1, ' ', index2 - (index1 + len1));
    strncat(buffer, str2, len2);
    memset(buffer + index2 + len2, ' ', TEXT_LCD_BUFFER_SIZE - (index2 + len2));
    buffer[TEXT_LCD_BUFFER_SIZE] = '\0';

    logger(INFO, "Writing line 2 \"%-16s\" into Text LCD device\n",
           buffer + 16);

    for (i = 0; i < TEXT_LCD_BUFFER_SIZE; i += 2) {
        value = (buffer[i] & 0xFF) << 8 | (buffer[i + 1] & 0xFF);
        outw(value, (unsigned int) fpga_addr[TEXT_LCD] + i);
    }
}
