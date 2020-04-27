#include "core.h"

const char DEVICE_PATHS[][21] = {DOT_DEVICE,      FND_DEVICE, LED_DEVICE,
                                 TEXT_LCD_DEVICE, KEY_DEVICE, SWITCH_DEVICE};

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

const unsigned char DOT_EMPTY[] = {
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
    devices[KEY] = open(KEY_DEVICE, O_RDONLY | O_NONBLOCK);
    devices[SWITCH] = open(SWITCH_DEVICE, O_RDWR);

    enum _devices dev;
    for (dev = 0; dev < DEVICES_CNT; dev++) {
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
    enum _devices dev;
    for (dev = 0; dev < DEVICES_CNT; dev++) {
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

void deviceLog(const enum _devices device, const enum _logLevel level,
               const char* format, ...) {
#ifndef _DEBUG_FLAG_
    if (level != ERROR) {
        return;
    }
#endif
    const char NAMES[][9] = {"DOT", "FND", "LED", "TEXT_LCD", "KEY", "SWITCH"};
    const char LEVELS[][8] = {"ERROR", "WARNING", "INFO"};

    printf("[%s] %s\t- ", LEVELS[level], NAMES[device]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void writeToDevice(const enum _devices device, const void* data,
                   const size_t size) {
    if (device == KEY || device == SWITCH) {
        deviceLog(device, ERROR, "Can not write to device\n");
        return;
    }

    if (device == LED) {
        *ledAddr = *((int*) data);
    } else {
        if (write(devices[device], data, size) < 0) {
            deviceLog(device, ERROR, "Write error\n");
            return;
        }
    }
}

void resetDevices() {
    dotReset();
    fndReset();
    ledReset();
    textLcdReset();
}

void dotPrintArray(const bool** data) {
    unsigned char processed[DOT_ROWS] = {'\0'};
    int i, j;

    for (i = 0; i < DOT_ROWS; i++) {
        for (j = 0; j < DOT_COLS; j++) {
            if (data[i][j]) {
                processed[i] += 1 << (DOT_COLS - 1 - j);
            }
        }
    }

    writeToDevice(DOT, processed, DOT_ROWS);

    deviceLog(DOT, INFO, "Printed array:\n");
#ifdef _DEBUG_FLAG_
    for (i = 0; i < DOT_ROWS; i++) {
        char row[DOT_COLS + 1] = {'\0'};
        for (j = 0; j < DOT_COLS; j++) {
            row[j] = data[i][j] ? '1' : '0';
        }
        row[DOT_COLS] = '\n';
        deviceLog(DOT, INFO, row);
    }
#endif
}

void dotPrintChar(const char data) {
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

void dotReset() {
    deviceLog(DOT, INFO, "Requested reset\n");

    writeToDevice(DOT, DOT_EMPTY, sizeof(DOT_EMPTY));
}

void fndPrint(const int data) {
    unsigned char digits[FND_MAX_DIGITS + 1] = {'\0'};
    int i, digit = 1;

    if (data < 0 || data > 9999) {
        deviceLog(FND, ERROR, "Value out of bound (0~9999): %d\n", data);
        return;
    }

    for (i = FND_MAX_DIGITS - 1; i >= 0; i--) {
        digits[i] = (data / digit) % 10;
        digit *= 10;
    }

    writeToDevice(FND, digits, FND_MAX_DIGITS);

    deviceLog(FND, INFO, "Printed value: %04d\n", data);
}

void fndReset() {
    deviceLog(FND, INFO, "Requested reset\n");

    fndPrint(0);
}

void ledPrint(const int data) {
    if (data < 0 || data > 255) {
        deviceLog(LED, ERROR, "Value out of bound (0~255): %d\n", data);
        return;
    }

    writeToDevice(LED, &data, sizeof(data));

    deviceLog(LED, INFO, "Printed value: %d\n", data);
}

void ledReset() {
    deviceLog(LED, INFO, "Requested reset\n");

    ledPrint(0);
}

void textLcdPrint(const char* data) {
    char processed[TEXT_LCD_MAX_LEN + 1] = {'\0'};
    int dataLength = strlen(data);

    if (dataLength > TEXT_LCD_MAX_LEN) {
        deviceLog(TEXT_LCD, WARNING, "String exceeded max length (32): %d\n",
                  dataLength);
        dataLength = TEXT_LCD_MAX_LEN;
    }

    strncpy(processed, data, TEXT_LCD_MAX_LEN);
    memset(processed + dataLength, ' ', TEXT_LCD_MAX_LEN - dataLength);

    writeToDevice(TEXT_LCD, processed, TEXT_LCD_MAX_LEN);

#ifdef _DEBUG_FLAG_
    processed[dataLength] = '\0';
    deviceLog(TEXT_LCD, INFO, "Printed value: %s\n", processed);
#endif
}

void textLcdReset() {
    deviceLog(TEXT_LCD, INFO, "Requested reset\n");

    textLcdPrint("");
}

enum _keys keyRead() {
    struct input_event keyBuffer[KEY_MAX_CNT];
    const int keyEventSize = sizeof(struct input_event);

    if (read(devices[KEY], keyBuffer, sizeof(keyBuffer)) >= keyEventSize &&
        keyBuffer[0].value == BUTTON_PRESSED) {
        switch (keyBuffer[0].code) {
            case KEY_VOLUMEDOWN:
            case KEY_VOLUMEUP:
            case KEY_BACK:
            case KEY_POWER:
                deviceLog(KEY, INFO, "Received input: %d\n", keyBuffer[0].code);
                return keyBuffer[0].code;
                break;
            default:
                break;
        }
    }
    return 0;
}

int switchRead() {
    unsigned char switchBuffer[SWITCH_CNT];
    int i, processed = 0;

    read(devices[SWITCH], &switchBuffer, sizeof(switchBuffer));
    for (i = 0; i < SWITCH_CNT; i++) {
        processed *= 10;
        if (switchBuffer[i] == BUTTON_PRESSED) {
            processed += 1;
        }
    }
    deviceLog(SWITCH, INFO, "Received input: %09d\n", processed);
    return processed;
}
