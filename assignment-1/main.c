#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;
extern struct _shmOutBuf* outputBuffer;

#define TEST_SLEEP 3  // FIXME: REMOVE LATER

void testDot() {  // FIXME: REMOVE LATER
    const bool testME[DOT_ROWS][DOT_COLS] = {
        {0, 0, 0, 0, 0, 0, 0},  // .......
        {0, 0, 0, 0, 0, 0, 0},  // .......
        {0, 0, 1, 0, 1, 0, 0},  // ..@.@..
        {0, 0, 1, 0, 1, 0, 0},  // ..@.@..
        {0, 0, 1, 0, 1, 0, 0},  // ..@.@..
        {0, 0, 0, 0, 0, 0, 0},  // .......
        {0, 1, 0, 0, 0, 1, 0},  // .@...@.
        {0, 1, 0, 0, 0, 1, 0},  // .@...@.
        {0, 0, 1, 1, 1, 0, 0},  // ..@@@..
        {0, 0, 0, 0, 0, 0, 0},  // .......
    };
    printf("\n========== DOT TEST ==========\n");
    dotPrintChar('A');
    sleep(TEST_SLEEP);
    dotPrintChar('!');
    sleep(TEST_SLEEP);
    dotPrintChar('1');
    sleep(TEST_SLEEP);
    dotPrintArray(testME);
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testFND() {  // FIXME: REMOVE LATER
    printf("\n========== FND TEST ==========\n");
    fndPrint(1234);
    sleep(TEST_SLEEP);
    fndPrint(-1);
    sleep(TEST_SLEEP);
    fndPrint(99999);
    sleep(TEST_SLEEP);
    fndPrint(123);
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testLED() {  // FIXME: REMOVE LATER
    printf("\n========== LED TEST ==========\n");
    ledPrint(255);
    sleep(TEST_SLEEP);
    ledPrint(0);
    sleep(TEST_SLEEP);
    ledPrint(999);
    sleep(TEST_SLEEP);
    ledPrint(96);
    sleep(TEST_SLEEP);
    printf("\n========== TEST END ==========\n");
}

void testTEXT_LCD() {  // FIXME: REMOVE LATER
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
    int semID = getSemaphore();
    getSharedMemory(SHM_KEY_1, (void**) &inputBuffer, sizeof(struct _shmInBuf));
    getSharedMemory(SHM_KEY_2, (void**) &outputBuffer,
                    sizeof(struct _shmOutBuf));

    openDevices();
    resetDevices();
    // sleep(TEST_SLEEP);

    // testDot();
    // testFND();
    // testLED();
    // testTEXT_LCD();

    switch (createForks()) {
        case MAIN:
            _main(semID);
            resetDevices();
            closeDevices();
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
    static enum _mode mode = CLOCK;
    struct _clockPayload clockPayload;
    while (true) {
        // wait for input's payload
        semop(semID, &p[SEM_INPUT_TO_MAIN], 1);

        switch (mode) {
            case CLOCK:
                clockPayload.resetClock = false;
                clockPayload.modeChanged = false;

                clockMode(&clockPayload);
                break;
            case COUNTER:
                break;
            case TEXT_EDITOR:
                break;
            case DRAW_BOARD:
                break;
        }

        // tell input payload is read
        semop(semID, &v[SEM_MAIN_TO_INPUT], 1);

        // tell output payload is ready
        semop(semID, &v[SEM_MAIN_TO_OUTPUT], 1);
        // wait for output to complete
        semop(semID, &p[SEM_OUTPUT_TO_MAIN], 1);

        usleep(300000);

        // sscanf(fromInput->buf, "%d", &pressedButtons);
        // for (i = 0; i < BUTTONS_CNT; i++) {
        //     if ((pressedButtons & (1 << i)) != 0) {
        //         switch (1 << i) {
        //             case SW1:
        //                 break;
        //             case SW2:
        //                 break;
        //             case SW3:
        //                 break;
        //             case SW4:
        //                 break;
        //             case SW5:
        //                 break;
        //             case SW6:
        //                 break;
        //             case SW7:
        //                 break;
        //             case SW8:
        //                 break;
        //             case SW9:
        //                 break;
        //             case PROG:
        //                 break;
        //             case VOL_UP:
        //                 mode = (mode + 1) % MODES_CNT;
        //                 break;
        //             case VOL_DOWN:
        //                 mode = (mode + MODES_CNT - 1) % MODES_CNT;
        //                 break;
        //             case BACK:
        //                 break;
        //         }
        //     }
        // }
    }
}

void throwError(const char* error) {
    perror(error);
    exit(1);
}

void clockMode(const struct _clockPayload* payload) {
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;

    time_t rawtime;
    time(&rawtime);
    const struct tm* timeinfo = localtime(&rawtime);
    const int hour = timeinfo->tm_hour;
    const int minute = timeinfo->tm_min;
    const int second = timeinfo->tm_sec;
    const int time = hour * 100 + minute;
    int fnd, leds;

    static int editHour, editMinute;
    static bool inEdit = false;

    if (payload->resetClock && inEdit) {
        inEdit = false;
    } else if (payload->modeChanged) {
        inEdit = !inEdit;
    }

    if (!inEdit) {
        leds = LED_1;
        fnd = time;
    } else {
        if (payload->modeChanged) {
            editHour = hour;
            editMinute = minute;
            leds = LED_3 | LED_4;
        } else if (second % 2 == 0) {
            leds = LED_3;
        } else {
            leds = LED_4;
        }

        if (payload->increaseMinute) {
            editMinute++;
            if (editMinute >= 60) {
                editHour++;
            }
        }
        if (payload->increaseHour) {
            editHour++;
        }

        editHour %= 24;
        editMinute %= 60;

        fnd = editHour * 100 + editMinute;
    }

    outputBuffer->ledBuffer = leds;
    outputBuffer->fndBuffer = fnd;

    // fndPrint(output);
}
