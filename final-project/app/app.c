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

int main(int argc, char* argv[]) {
    int i;
    char letter;
    // Open device file
    const int fd = open(DEVICE_FILE(DEVICE_NAME), O_RDWR);
    if (fd == -1) {
        printf("Error opening device file\n");
        return -1;
    }

    // write(fd, NULL, 0);
    for (i = 0; i < 5; i++) {
        ioctl(fd, IOCTL_READ_LETTER);
        read(fd, &letter, 1);
        printf("\n[APP] read: %c\n", letter);
    }

    // Close device file
    close(fd);

    return 0;
}
