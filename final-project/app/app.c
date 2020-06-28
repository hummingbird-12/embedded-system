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

#define DEVICE_FILE(X) ("/dev/" DEVICE_NAME)
#define WORD_MAX_LEN 20

struct _payload {
    char word[WORD_MAX_LEN];
    int status;
    int score;
} payload;

int main(int argc, char* argv[]) {
    // Open device file
    const int fd = open(DEVICE_FILE(DEVICE_NAME), O_RDWR);
    if (fd == -1) {
        printf("Error opening device file\n");
        return -1;
    }

    do {
        ioctl(fd, IOCTL_READ_LETTER);
        read(fd, &payload, sizeof(payload));
        printf("\n[APP] read: %s\tscore %d\n", payload.word, payload.score);
    } while (payload.status != -1);

    // Close device file
    close(fd);

    return 0;
}
