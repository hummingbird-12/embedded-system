#include "core.h"

const char DEVICE_PATHS[][20] = {DOT_DEVICE, FND_DEVICE, LED_DEVICE,
                                 TEXT_LCD_DEVICE};

const unsigned char DOT_1[] = {
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0001 << 4) + DOT_1100,  // ..@ @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0000 << 4) + DOT_1100,  // ... @@..
    (DOT_0011 << 4) + DOT_1111,  // .@@ @@@@
    (DOT_0011 << 4) + DOT_1111,  // .@@ @@@@
};

const unsigned char DOT_A[] = {
    (DOT_0001 << 4) + DOT_1100,  // ..@ @@..
    (DOT_0011 << 4) + DOT_0110,  // .@@ .@@.
    (DOT_0110 << 4) + DOT_0011,  // @@. ..@@
    (DOT_0110 << 4) + DOT_0011,  // @@. ..@@
    (DOT_0110 << 4) + DOT_0011,  // @@. ..@@
    (DOT_0111 << 4) + DOT_1111,  // @@@ @@@@
    (DOT_0111 << 4) + DOT_1111,  // @@@ @@@@
    (DOT_0110 << 4) + DOT_0011,  // @@. ..@@
    (DOT_0110 << 4) + DOT_0011,  // @@. ..@@
    (DOT_0110 << 4) + DOT_0011   // @@. ..@@
};

const unsigned char DOT_BLANK[10] = {
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000,  // ... ....
    DOT_0000   // ... ....
};

int devices[DEVICES_CNT];
unsigned long* FPGA_ADDR;
unsigned char* ledAddr;

void openDevices() {
    devices[DOT] = open(DOT_DEVICE, O_WRONLY);
    devices[FND] = open(FND_DEVICE, O_RDWR);
    devices[LED] = open(LED_DEVICE, O_RDWR | O_SYNC);
    devices[TEXT_LCD] = open(TEXT_LCD_DEVICE, O_WRONLY);

    enum _DEVICES dev;
    for (dev = DOT; dev < DEVICES_CNT; dev++) {
        if (devices[dev] < 0) {
            deviceLog(dev, ERROR, "Error while opening device: %s\n",
                      DEVICE_PATHS[dev]);
            return;
        }
    }

    FPGA_ADDR =
        (unsigned long*) mmap(NULL, FPGA_MAP_LENGTH, PROT_READ | PROT_WRITE,
                              MAP_SHARED, devices[LED], FPGA_BASE_ADDR);
    if (FPGA_ADDR == MAP_FAILED) {
        deviceLog(LED, ERROR, "Error while mapping FPGA board\n");
        return;
    }
    ledAddr = (unsigned char*) ((void*) FPGA_ADDR + LED_ADDR_OFFSET);
}

void closeDevices() {
    enum _DEVICES dev;
    for (dev = DOT; dev < DEVICES_CNT; dev++) {
        if (devices[dev] < 0) {
            deviceLog(dev, WARNING, "Error while closing device: %s\n",
                      DEVICE_PATHS[dev]);
            return;
        }
    }

    if (munmap((void*) FPGA_ADDR, FPGA_MAP_LENGTH) < 0) {
        deviceLog(LED, WARNING, "Error while unmapping FPGA board\n");
        return;
    }
}

void deviceLog(const enum _DEVICES device, const enum _LOG_LEVEL level,
               const char* format, ...) {
#ifdef _DEBUG_FLAG_
    const char NAMES[][9] = {"DOT", "FND", "LED", "TEXT_LCD"};
    const char LEVELS[][8] = {"ERROR", "WARNING", "INFO"};

    printf("[%s] %s - ", NAMES[device], LEVELS[level]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#elif defined(_TESTING_FLAG_)
    if (level == ERROR) {
        printf("[IMPORTANT] A device error has occured!\n");
        printf(
            "Please enable `_DEBUG_FLAG_` in `devices.h` to show device "
            "logs.\n");
    }
#endif
}

void dotPrint(const char data) {
    deviceLog(DOT, INFO, "Requested print value: '%c'\n", data);

    if (!isalnum(data)) {
        deviceLog(DOT, ERROR, "Invalid character: '%c'\n", data);
        return;
    }

    if (data == '1') {
        if (write(devices[DOT], DOT_1, sizeof(DOT_1)) < 0) {
            deviceLog(DOT, ERROR, "Write error\n");
            return;
        }
    } else if (data == 'A') {
        if (write(devices[DOT], DOT_A, sizeof(DOT_A)) < 0) {
            deviceLog(DOT, ERROR, "Write error\n");
            return;
        }
    }

    deviceLog(DOT, INFO, "Printed value: %c\n", data);
}

void fndPrint(const int data) {
    deviceLog(FND, INFO, "Requested print value: %d\n", data);

    if (data < 0 || data > 9999) {
        deviceLog(FND, ERROR, "Value out of bound (0~9999): %d\n", data);
        return;
    }

    unsigned char digits[FND_MAX_DIGITS + 1] = {'\0'};
    int digit = 1, i;
    for (i = FND_MAX_DIGITS - 1; i >= 0; i--) {
        digits[i] = (data / digit) % 10;
        digit *= 10;
    }

    if (write(devices[FND], digits, FND_MAX_DIGITS) < 0) {
        deviceLog(FND, ERROR, "Write error\n");
        return;
    }

    deviceLog(FND, INFO, "Printed value: %d\n", data);
}

void ledPrint(const int data) {
    if (data < 0 || data > 255) {
        deviceLog(LED, ERROR, "Value out of bound (0~255): %d\n", data);
        return;
    }

    *ledAddr = data;

    deviceLog(LED, INFO, "Printed value: %d\n", data);
}

// void textLcdPrint()
