#include "core.h"

static char* fpga_addr[FPGA_DEVICES_CNT];

static const int FPGA_HW_ADDR[] = {BUZZER_ADDRESS, DOT_ADDRESS,
                                   FND_ADDRESS,    LED_ADDRESS,
                                   SWITCH_ADDRESS, TEXT_LCD_ADDRESS};

/*
 * IO-maps the FPGA devices.
 */
void fpga_iomap_devices(void) {
    logger(INFO, "[fpga] IO-mapping FPGA devices\n");

    fpga_addr[BUZZER] = ioremap(FPGA_HW_ADDR[BUZZER], 0x1);
    fpga_addr[DOT] = ioremap(FPGA_HW_ADDR[DOT], 0xA);
    fpga_addr[FND] = ioremap(FPGA_HW_ADDR[FND], 0x2);
    fpga_addr[LED] = ioremap(FPGA_HW_ADDR[LED], 0x1);
    fpga_addr[SWITCH] = ioremap(FPGA_HW_ADDR[SWITCH], 0x18);
    fpga_addr[TEXT_LCD] = ioremap(FPGA_HW_ADDR[TEXT_LCD], 0x32);

    const char* FPGA_DEVICES_STR[] = {"BUZZER", "DOT",    "FND",
                                      "LED",    "SWITCH", "TEXT_LCD"};
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
