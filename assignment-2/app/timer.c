#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../module/timer_device.h"

#define ARGS_CNT 4
#define DEVICE_FILE(X) ("/dev/" DEVICE_NAME)

int main(int argc, char* argv[]) {
    if (argc != ARGS_CNT) {
        printf("Incorrect set of arguments!\n");
        return -1;
    }

    const int timerInterval = atoi(argv[1]);
    const int timerCount = atoi(argv[2]);
    const int timerInit = atoi(argv[3]);

    if (timerInterval == 0 || timerCount == 0 || timerInit == 0) {
        printf("Error parsing arguments!\n");
        return -1;
    }

    const int fd = open(DEVICE_FILE(DEVICE_NAME), O_WRONLY);
    if (fd == -1) {
        printf("Error opening device file\n");
        return -1;
    }

    char payload[11] = {'\0'};
    sprintf(payload, "%03d%03d%04d", timerInterval, timerCount, timerInit);
    ioctl(fd, IOCTL_SET_OPTION, payload);
    ioctl(fd, IOCTL_COMMAND);

    close(fd);

    return 0;
}
