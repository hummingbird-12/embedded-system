#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *fromInput, *toOutput;

enum _MODE { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD } mode = CLOCK;

#define TEST_SLEEP 3

void testDot() {
    printf("\n========== DOT TEST ==========\n");
    dotPrint('A');
    sleep(TEST_SLEEP);
    dotPrint('!');
    sleep(TEST_SLEEP);
    dotPrint('3');
    sleep(TEST_SLEEP);
    dotPrint('1');
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testFND() {
    printf("\n========== FND TEST ==========\n");
    fndPrint(1234);
    sleep(TEST_SLEEP);
    fndPrint(123);
    sleep(TEST_SLEEP);
    fndPrint(99999);
    sleep(TEST_SLEEP);
    fndPrint(-1);
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testLED() {
    printf("\n========== LED TEST ==========\n");
    ledPrint(255);
    sleep(TEST_SLEEP);
    ledPrint(0);
    sleep(TEST_SLEEP);
    ledPrint(999);
    sleep(TEST_SLEEP);
    ledPrint(-1);
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testTEXT_LCD() {
    printf("\n========== TEXT_LCD TEST ==========\n");
    textLcdPrint("Hi there");
    sleep(TEST_SLEEP);
    textLcdPrint("The quick brown fox jumped");
    sleep(TEST_SLEEP);
    textLcdPrint("The quick brown fox jumped over a lazy dog.");
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

int main() {
    // int semID = getSemaphore();
    // getSharedMemory(SHM_KEY_1, &fromInput);
    // getSharedMemory(SHM_KEY_2, &toOutput);

    openDevices();

    testDot();
    // testFND();
    // testLED();
    // testTEXT_LCD();

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
