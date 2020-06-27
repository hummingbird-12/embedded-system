#include "core.h"

static char* fpga_addr[FPGA_DEVICES_CNT];

static const int FPGA_HW_ADDR[] = {DOT_ADDRESS, FND_ADDRESS, LED_ADDRESS,
                                   SWITCH_ADDRESS, TEXT_LCD_ADDRESS};

/*
 * IO-maps the FPGA devices.
 */
void fpga_iomap_devices(void) {
    logger(INFO, "[fpga] IO-mapping FPGA devices\n");

    fpga_addr[DOT] = ioremap(FPGA_HW_ADDR[DOT], 0xA);
    fpga_addr[FND] = ioremap(FPGA_HW_ADDR[FND], 0x2);
    fpga_addr[LED] = ioremap(FPGA_HW_ADDR[LED], 0x1);
    fpga_addr[SWITCH] = ioremap(FPGA_HW_ADDR[SWITCH], 0x18);
    fpga_addr[TEXT_LCD] = ioremap(FPGA_HW_ADDR[TEXT_LCD], 0x32);

    const char* FPGA_DEVICES_STR[] = {"DOT", "FND", "LED", "SWITCH",
                                      "TEXT_LCD"};
    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        if (fpga_addr[dev] == NULL) {
            logger(ERROR, "Failed to IO-map device %s\n",
                   FPGA_DEVICES_STR[dev]);
        }
    }
}

/*
 * IO-unmaps the FPGA devices.
 */
void fpga_iounmap_devices(void) {
    logger(INFO, "[fpga] IO-unmapping FPGA devices\n");

    fpga dev;
    for (dev = 0; dev < FPGA_DEVICES_CNT; dev++) {
        iounmap(fpga_addr[dev]);
    }
}

/*
 *
 */
void fpga_dot_write(const char letter) {
    int i;

    logger(INFO, "Printing '%c' into Dot Matrix device\n", letter);

    for (i = 0; i < 10; i++) {
        outw(fpga_dot_letters[letter - 'A'][i] & 0x7F,
             (unsigned int) fpga_addr[DOT] + i * 2);
    }
}

/*
 *
 */
void fpga_fnd_write(const int score) {
    int i;
    unsigned short value = 0;

    logger(INFO, "[fpga] Printing score %04d into FND device\n", score);

    for (i = 0; i < 4; i++, score /= 10) {
        value += (score % 10) << (4 * i);
    }

    outw(value, (unsigned int) fnd_addr);
}

/*
 *
 */
void fpga_led_write(const int lives) {
    const unsigned short value = ((lives > 0) ? (0xFF << (8 - digit)) : 0);

    logger(INFO, "Turning on LED %d of LED device\n", digit);

    outw(value, (unsigned int) fpga_addr[LED]);
}

/*
 *
 */
int fpga_switch_read() {
    int i;
    unsigned short int _s_value;

    for (i = 0; i < length; i++) {
        _s_value = inw((unsigned int) fpga_addr[SWITCH] + i * 2);
        if (_s_value & 0xFF != 0) {
            return i;
        }
    }
}

/*
 *
 */
void fpga_text_lcd_write(const char* available) {
    int i;
    unsigned short int value;
    unsigned char buffer[TEXT_LCD_BUFFER_SIZE + 1] = {0};

    memset(buffer, ' ', TEXT_LCD_BUFFER_SIZE);

    for (i = 0; i < 13; i++) {
        buffer[i + 2] = (available[i] != 0 ? 'A' + i : ' ');
    }

    buffer[16] = '\0';
    logger(INFO, "Writing line 1 \"%-16s\" into Text LCD device\n", buffer);
    buffer[16] = ' ';

    for (i = 13; i < 26; i++) {
        buffer[i + 5] = (available[i] != 0 ? 'A' + i : ' ');
    }

    logger(INFO, "Writing line 2 \"%-16s\" into Text LCD device\n",
           buffer + 16);

    for (i = 0; i < TEXT_LCD_BUFFER_SIZE; i += 2) {
        value = (buffer[i] & 0xFF) << 8 | (buffer[i + 1] & 0xFF);
        outw(value, (unsigned int) fpga_addr[TEXT_LCD] + i);
    }
}
