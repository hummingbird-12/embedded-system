#include "core.h"

const char DEVICE_PATHS[][20] = {DOT_DEVICE, FND_DEVICE, LED_DEVICE,
                                 TEXT_LCD_DEVICE};
int devices[DEVICES_CNT];

void openDevices() {
    devices[DOT] = open(DOT_DEVICE, O_WRONLY);
    devices[FND] = open(FND_DEVICE, O_RDWR);
    devices[LED] = open(LED_DEVICE, O_RDWR | O_SYNC);
    devices[TEXT_LCD] = open(TEXT_LCD_DEVICE, O_WRONLY);

    int i;
    for (i = 0; i < DEVICES_CNT; i++) {
        if (devices[i] < 0) {
            printf("Error while opening device: %s\n", DEVICE_PATHS[i]);
            return;
        }
    }
}

void closeDevices() {
    int i;
    for (i = 0; i < DEVICES_CNT; i++) {
        if (close(devices[i]) < 0) {
            printf("Error while closing device: %s\n", DEVICE_PATHS[i]);
        }
    }
}

void printError(enum _DEVICES device, const enum _LOG_LEVEL level,
                const char* format, ...) {
    const char NAMES[][9] = {"DOT", "FND", "LED", "TEXT_LCD"};
    const char LEVELS[][8] = {"ERROR", "WARNING"};
    va_list args;
    va_start(args, format);

    printf("[%s] %s - ", NAMES[device], LEVELS[level]);
    vprintf(format, args);
    va_end(args);
}

void fndPrint(char* str) {
    if (strlen(str) > FND_MAX_DIGITS) {
        printError(FND, WARNING, "Trimming to %d digits!", FND_MAX_DIGITS);
        str[FND_MAX_DIGITS] = '\0';
    }

    unsigned char data[FND_MAX_DIGITS] = {'\0'};
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] < '0' || str[i] > '9') {
            printf("[FND] [ERROR] Invalid value!\n");
            return;
        }
        data[i] = str[i] - '0';
    }

    if (write(devices[FND], &data, FND_MAX_DIGITS) < 0) {
        printf("[FND] [ERROR] Write error!\n");
        return;
    }
}
