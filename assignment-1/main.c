#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *fromInput, *toOutput;

enum _MODE { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD } mode = CLOCK;

int main() {
    int semID = getSemaphore();
    getSharedMemory(SHM_KEY_1, &fromInput);
    getSharedMemory(SHM_KEY_2, &toOutput);

    switch (createForks()) {
        case MAIN:
            _main(semID);
            break;
        case INPUT:
            input(semID);
            break;
        case OUTPUT:
            output(semID);
            break;
    }

    return 0;
}

void _main(const int semID) {
    while (true) {
        // wait until input
        semop(semID, &p[SEM_INPUT_READ], 1);

        // strcpy(toOutput->buf, "Input received: ");
        int pressedButtons, i;
        sscanf(fromInput->buf, "%d", &pressedButtons);
        for (i = 0; i < BUTTONS_COUNT; i++) {
            if ((pressedButtons & (1 << i)) != 0) {
                switch (1 << i) {
                    case SW1:
                        break;
                    case SW2:
                        break;
                    case SW3:
                        break;
                    case SW4:
                        break;
                    case SW5:
                        break;
                    case SW6:
                        break;
                    case SW7:
                        break;
                    case SW8:
                        break;
                    case SW9:
                        break;
                    case PROG:
                        break;
                    case VOL_UP:
                        mode = (mode + 1) % MODES_COUNT;
                        break;
                    case VOL_DOWN:
                        mode = (mode + MODES_COUNT - 1) % MODES_COUNT;
                        break;
                    case BACK:
                        break;
                }
            }
        }
        strcat(toOutput->buf, "\n");
        toOutput->nread = strlen(toOutput->buf) * sizeof(char);

        // tell output it's ready
        semop(semID, &v[SEM_MAIN_READY], 1);
    }
}

void throwError(const char* error) {
    perror(error);
    exit(1);
}
