#include "core.h"

char* fpga_addr[FPGA_DEVICES_CNT];

static const char* FPGA_DEVICES_STR[] = {"DOT", "FND", "LED", "TEXT_LCD"};
static const int FPGA_HW_ADDR[] = {DOT_ADDRESS, FND_ADDRESS, LED_ADDRESS,
                                   TEXT_LCD_ADDRESS};

void fpga_iomap_devices(void) {
    fpga dev;

    logger(INFO, "IO-mapping FPGA devices\n");

    fpga_addr[DOT] = ioremap(FPGA_HW_ADDR[DOT], 0x10);
    fpga_addr[FND] = ioremap(FPGA_HW_ADDR[FND], 0x4);
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
