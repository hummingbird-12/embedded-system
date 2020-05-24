#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "driver_specs.h"

#define ARGS_CNT 4
#define DEVICE_FILE(X) ("/dev/" DEVICE_NAME)

int main(int argc, char* argv[]) {
    int i, hasNonZero;

    // Argument checking and parsing
    if (argc != ARGS_CNT) {
        printf("Incorrect set of arguments!\n");
        printf(
            "Usage: ./timer TIMER_INTERVAL[1-100] TIMER_CNT[1-100] "
            "TIMER_INIT[0001-8000]\n");
        return -1;
    }

    const int timerInterval = atoi(argv[1]);
    const int timerCount = atoi(argv[2]);
    const int timerInit = atoi(argv[3]);

    if (timerInterval == 0 || timerCount == 0) {
        printf("Error parsing arguments!\n");
        return -1;
    }

    // Check for correct boundaries
    if (timerInterval < 1 || timerInterval > 100) {
        printf("TIMER_INTERVAL should be between 1 and 100, including.\n");
        return -1;
    }

    // Check for correct boundaries
    if (timerCount < 1 || timerCount > 100) {
        printf("TIMER_CNT should be between 1 and 100, including.\n");
        return -1;
    }

    // Check for TIMER_INIT argument
    hasNonZero = 0;
    if (strlen(argv[3]) != 4) {
        printf("TIMER_INIT must consist of 4 digits.\n");
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (!isdigit(argv[3][i]) || argv[3][i] == '9') {
            printf(
                "TIMER_INIT must only have digits between 0 and 8, "
                "including.\n");
            return -1;
        }
        if (argv[3][i] - '0' > 0) {
            if (hasNonZero != 0) {
                printf("TIMER_INIT must only have a single non-zero digit.\n");
                return -1;
            }
            hasNonZero = 1;
        }
    }
    if (hasNonZero == 0) {
        printf("TIMER_INIT must have a single non-zero digit.\n");
        return -1;
    }

    // Open device file
    const int fd = open(DEVICE_FILE(DEVICE_NAME), O_WRONLY);
    if (fd == -1) {
        printf("Error opening device file\n");
        return -1;
    }

    char payload[11] = {'\0'};
    sprintf(payload, "%03d%03d%04d", timerInterval, timerCount, timerInit);

    /*
     * Pass arguments with `IOCTL_SET_OPTION`
     * and start timer with `IOCTL_COMMAND`
     */
    ioctl(fd, IOCTL_SET_OPTION, payload);
    ioctl(fd, IOCTL_COMMAND);

    // Close device file
    close(fd);

    return 0;
}
