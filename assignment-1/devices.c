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
#ifndef _DEBUG_FLAG_
    if (level != ERROR) {
        return;
    }
#endif
    const char NAMES[][9] = {"DOT", "FND", "LED", "TEXT_LCD"};
    const char LEVELS[][8] = {"ERROR", "WARNING", "INFO"};

    printf("[%s] %s - ", NAMES[device], LEVELS[level]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void writeToDevice(const enum _DEVICES device, const void* data,
                   const size_t size) {
    if (device == LED) {
        *ledAddr = *((int*) data);
    } else {
        if (write(devices[device], data, size) < 0) {
            deviceLog(device, ERROR, "Write error\n");
            return;
        }
    }
}

void dotPrint(const char data) {
    deviceLog(DOT, INFO, "Requested print value: '%c'\n", data);

    switch (data) {
        case '1':
            writeToDevice(DOT, DOT_1, sizeof(DOT_1));
            break;
        case 'A':
            writeToDevice(DOT, DOT_A, sizeof(DOT_A));
            break;
        default:
            deviceLog(DOT, ERROR, "Not a valid character (1 or A): '%c'\n",
                      data);
            return;
            break;
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

    writeToDevice(FND, digits, FND_MAX_DIGITS);

    deviceLog(FND, INFO, "Printed value: %d\n", data);
}

void ledPrint(const int data) {
    deviceLog(LED, INFO, "Requested print value: %d\n", data);

    if (data < 0 || data > 255) {
        deviceLog(LED, ERROR, "Value out of bound (0~255): %d\n", data);
        return;
    }

    writeToDevice(LED, &data, sizeof(data));

    deviceLog(LED, INFO, "Printed value: %d\n", data);
}

void textLcdPrint(const char* data) {
    deviceLog(TEXT_LCD, INFO, "Requested print value: %s\n", data);

    int dataLength = strlen(data);
    if (dataLength > TEXT_LCD_MAX_LEN) {
        deviceLog(TEXT_LCD, WARNING, "String exceeded max length (32): %d\n",
                  dataLength);
        dataLength = TEXT_LCD_MAX_LEN;
    }

    char processed[TEXT_LCD_MAX_LEN + 1] = {'\0'};
    strncpy(processed, data, TEXT_LCD_MAX_LEN);
    memset(processed + dataLength, ' ', TEXT_LCD_MAX_LEN - dataLength);

    writeToDevice(TEXT_LCD, processed, TEXT_LCD_MAX_LEN);

#ifdef _DEBUG_FLAG_
    processed[dataLength] = '\0';
    deviceLog(TEXT_LCD, INFO, "Printed value: %s\n", processed);
#endif
}
