#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf* fromInput;

void input(const int semID) {
    int keyFD, switchFD, i, pressedButtons;
    const char* keyDevice = "/dev/input/event0";
    const char* switchDevice = "/dev/fpga_push_switch";
    struct input_event keyBuffer[KEY_MAX_COUNT];
    unsigned char switchBuffer[SWITCH_COUNT];

    const int keyEventSize = sizeof(struct input_event);

    if ((keyFD = open(keyDevice, O_RDONLY | O_NONBLOCK)) == -1) {
        printf("%s is not a vaild device.\n\n", keyDevice);
        throwError("Error opening device!");
    }

    if ((switchFD = open(switchDevice, O_RDWR)) == -1) {
        printf("%s is not a vaild device.\n\n", switchDevice);
        throwError("Error opening device!");
    }

    while (true) {
        if (read(keyFD, keyBuffer, sizeof(keyBuffer)) >= keyEventSize) {
            if (keyBuffer[0].value == BUTTON_PRESSED) {
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

                if (pressedButtons != 0) {
                    sprintf(fromInput->buf, "%d", pressedButtons);
                    fromInput->nread = strlen(fromInput->buf);

                    // tell input is complete
                    semop(semID, &v[SEM_INPUT_READ], 1);
                    // wait until output
                    semop(semID, &p[SEM_OUTPUT_WRITE], 1);

                    memset(fromInput->buf, '\0', SHM_SIZE);
                }
            }
        }

        read(switchFD, &switchBuffer, sizeof(switchBuffer));
        pressedButtons = 0;
        for (i = 0; i < SWITCH_COUNT; i++) {
            if (switchBuffer[i] == BUTTON_PRESSED) {
                pressedButtons |= 1 << i;
            }
        }

        if (pressedButtons != 0) {
            sprintf(fromInput->buf, "%d", pressedButtons);
            fromInput->nread = strlen(fromInput->buf);

            // tell input is complete
            semop(semID, &v[SEM_INPUT_READ], 1);
            // wait until output
            semop(semID, &p[SEM_OUTPUT_WRITE], 1);

            memset(fromInput->buf, '\0', SHM_SIZE);
        }

        usleep(200000);
    }
}
