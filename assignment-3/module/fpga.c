#include "core.h"
#include "fpga_dot.h"

static char* fnd_addr;

/*
 * IO-maps the FPGA's FND device.
 */
void fpga_iomap_fnd(void) {
    logger(INFO, "IO-mapping FND device\n");

    fnd_addr = ioremap(FND_ADDRESS, 0x2);

    if (fnd_addr == NULL) {
        logger(ERROR, "Failed to IO-map FND device\n");
    }
}

/*
 * IO-unmaps the FPGA's FND device.
 */
void fpga_iounmap_fnd(void) {
    logger(INFO, "IO-unmapping FND device\n");

    iounmap(fnd_addr);
}

/*
 * Prints time using given `minutes` and `seconds` in FPGA's FND device.
 * `minutes` should be between 0 and 59, including.
 * `seconds` should be between 0 and 59, including.
 */
void fpga_fnd_write(const int minutes, const int seconds) {
    int i, concat = minutes * 100 + seconds;
    unsigned short value = 0;

    for (i = 0; i < 4; i++, concat /= 10) {
        value += (concat % 10) << (4 * i);
    }

    logger(INFO, "Printing time %02d:%02d into FND device\n", minutes, seconds);

    outw(value, (unsigned int) fnd_addr);
}
