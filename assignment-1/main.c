#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *fromInput, *toOutput;

enum _MODE { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD } mode = CLOCK;

void testDot() {
    printf("\n========== DOT TEST ==========\n");
    dotPrint('A');
    sleep(2);
    dotPrint('!');
    sleep(2);
    dotPrint('3');
    sleep(2);
    dotPrint('1');
    sleep(2);
    printf("\n========== TEST END ==========\n");
}

void testFND() {
    printf("\n========== FND TEST ==========\n");
    fndPrint(1234);
    sleep(2);
    fndPrint(123);
    sleep(2);
    fndPrint(99999);
    sleep(2);
    fndPrint(-1);
    sleep(2);
    printf("\n========== TEST END ==========\n");
}

void testLED() {
    printf("\n========== LED TEST ==========\n");
    ledPrint(255);
    sleep(2);
    ledPrint(0);
    sleep(2);
    ledPrint(999);
    sleep(2);
    ledPrint(-1);
    sleep(2);
    printf("\n========== TEST END ==========\n");
}

int main() {
    // int semID = getSemaphore();
    // getSharedMemory(SHM_KEY_1, &fromInput);
    // getSharedMemory(SHM_KEY_2, &toOutput);

    openDevices();

    // testDot();
    testFND();
    // testLED();

    closeDevices();

    // switch (createForks()) {
    //     case MAIN:
    //         _main(semID);
    //         break;
    //     case INPUT:
    //         input(semID);
    //         break;
    //     case OUTPUT:
    //         output(semID);
    //         break;
    // }

    return 0;
}

void _main(const int semID) {
    bool inputLocked = false;
    while (true) {
        if ((fromInput->buf)[0] != '\0') {
            inputLocked = true;
        }

        int pressedButtons, i;
        sscanf(fromInput->buf, "%d", &pressedButtons);
        for (i = 0; i < BUTTONS_CNT; i++) {
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
                        mode = (mode + 1) % MODES_CNT;
                        break;
                    case VOL_DOWN:
                        mode = (mode + MODES_CNT - 1) % MODES_CNT;
                        break;
                    case BACK:
                        break;
                }
            }
        }

        clockMode();

        // tell output it's ready
        semop(semID, &p[SEM_MAIN_READY], 1);

        if (inputLocked) {
            memset(fromInput->buf, '\0', SHM_SIZE);
            semop(semID, &v[SEM_INPUT_READY], 1);
            inputLocked = false;
        }
    }
}

void throwError(const char* error) {
    perror(error);
    exit(1);
}

void clockMode() {
    time_t rawtime;
    struct tm* timeinfo;
    char output[5] = {'\0'};

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(output, "%02d%02d", timeinfo->tm_hour, timeinfo->tm_min);

    // fndPrint(output);
}
