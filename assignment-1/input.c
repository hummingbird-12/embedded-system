#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf* fromInput;

void input(const int semID) {
    int keyFD, switchFD;
    const char* keyDevice = "/dev/input/event0";
    const char* switchDevice = "/dev/fpga_push_switch";

    if ((keyFD = open(keyDevice, O_RDONLY | O_NONBLOCK)) == -1) {
        printf("%s is not a vaild device.\n\n", keyDevice);
        throwError("Error opening device!");
    }

    if ((switchFD = open(switchDevice, O_RDWR)) == -1) {
        printf("%s is not a vaild device.\n\n", switchDevice);
        throwError("Error opening device!");
    }

    while (true) {
        readKeys(semID, keyFD);
        readSwitches(semID, switchFD);

        usleep(200000);
    }
}

void readKeys(const int semID, const int keyFD) {
    struct input_event keyBuffer[KEY_MAX_CNT];
    const int keyEventSize = sizeof(struct input_event);
    int pressedButtons = 0;

    if (read(keyFD, keyBuffer, sizeof(keyBuffer)) >= keyEventSize &&
        keyBuffer[0].value == BUTTON_PRESSED) {
        switch (keyBuffer[0].code) {
            case 114:
                pressedButtons = VOL_DOWN;
                break;
            case 115:
                pressedButtons = VOL_UP;
                break;
            case 116:
                pressedButtons = PROG;
                break;
            case 158:
                pressedButtons = BACK;
                break;
            default:
                pressedButtons = 0;
                break;
        }

        writeToSHM(semID, pressedButtons);
    }
}

void readSwitches(const int semID, const int switchFD) {
    unsigned char switchBuffer[SWITCH_CNT];
    int i, pressedButtons = 0;

    read(switchFD, &switchBuffer, sizeof(switchBuffer));
    for (i = 0; i < SWITCH_CNT; i++) {
        if (switchBuffer[i] == BUTTON_PRESSED) {
            pressedButtons |= 1 << i;
        }
    }

    writeToSHM(semID, pressedButtons);
}

void writeToSHM(const int semID, const int pressedButtons) {
    if (pressedButtons != 0) {
        sprintf(fromInput->buf, "%d", pressedButtons);
        fromInput->nread = strlen(fromInput->buf);

        // Wait for main process
        semop(semID, &p[SEM_INPUT_READY], 1);
    }
}
