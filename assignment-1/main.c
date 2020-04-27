#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;
extern struct _shmOutBuf* outputBuffer;

const char TEXT_EDITOR_ALPHA[][3] = {
    "",                   // -
    ".QZ", "ABC", "DEF",  // SW(1)~SW(3)
    "GHI", "JKL", "MNO",  // SW(4)~SW(6)
    "PRS", "TUV", "WXY"   // SW(7)~SW(9)
};

int main() {
    int semID = getSemaphore();
    int shmInID = getSharedMemory(SHM_KEY_1, (void**) &inputBuffer,
                                  sizeof(struct _shmInBuf));
    int shmOutID = getSharedMemory(SHM_KEY_2, (void**) &outputBuffer,
                                   sizeof(struct _shmOutBuf));
    initializeSharedMemory();

    openDevices();

    switch (createForks()) {
        case MAIN:
            _main(semID);
            killChildProcesses();
            closeDevices();
            removeIpcObjects(semID, shmInID, shmOutID);
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
    bool modeChanged, quitFlag = false;

    clockPayload clockPl;
    counterPayload counterPl;
    textEditorPayload textEditorPl;
    drawBoardPayload drawBoardPl;

    while (true) {
        // wait for input's payload
        semop(semID, &p[SEM_INPUT_TO_MAIN], 1);

        modeChanged = false;
        if (inputBuffer->hasInput) {
            switch (inputBuffer->key) {
                case KEY_VOLUMEDOWN:
                    mode = (mode + MODES_CNT - 1) % MODES_CNT;
                    modeChanged = true;
                    break;
                case KEY_VOLUMEUP:
                    mode = (mode + 1) % MODES_CNT;
                    modeChanged = true;
                    break;
                case KEY_BACK:
                    quitFlag = true;
                    break;
                case KEY_POWER:
                    break;
                default:
                    break;
            }
            if (quitFlag) {
                break;
            }
        }

        switch (mode) {
            case CLOCK:
                clockPl = (clockPayload){false, false, false, false, false};

                if (modeChanged) {
                    clockPl.firstPayload = true;
                }

                if (inputBuffer->switches[1]) {
                    clockPl.toggleEdit = true;
                } else if (inputBuffer->switches[2]) {
                    clockPl.resetClock = true;
                } else if (inputBuffer->switches[3]) {
                    clockPl.increaseHour = true;
                } else if (inputBuffer->switches[4]) {
                    clockPl.increaseMinute = true;
                }

                clockMode(&clockPl);
                break;
            case COUNTER:
                counterPl = (counterPayload){false, false, false, false, false};

                if (modeChanged) {
                    counterPl.firstPayload = true;
                }

                if (inputBuffer->switches[1]) {
                    counterPl.changeRadix = true;
                } else if (inputBuffer->switches[2]) {
                    counterPl.increase2 = true;
                } else if (inputBuffer->switches[3]) {
                    counterPl.increase1 = true;
                } else if (inputBuffer->switches[4]) {
                    counterPl.increase0 = true;
                }

                counterMode(&counterPl);
                break;
            case TEXT_EDITOR:
                textEditorPl = (textEditorPayload){.firstPayload = false,
                                                   .clearText = false,
                                                   .triggerAlNum = false,
                                                   .putSpace = false};
                memset(textEditorPl.keypad, false, sizeof(textEditorPl.keypad));

                if (modeChanged) {
                    textEditorPl.firstPayload = true;
                }

                if (inputBuffer->switches[2] && inputBuffer->switches[3]) {
                    textEditorPl.clearText = true;
                } else if (inputBuffer->switches[5] &&
                           inputBuffer->switches[6]) {
                    textEditorPl.triggerAlNum = true;
                } else if (inputBuffer->switches[8] &&
                           inputBuffer->switches[9]) {
                    textEditorPl.putSpace = true;
                }
                memcpy(textEditorPl.keypad, inputBuffer->switches,
                       sizeof(SWITCH_CNT + 1));

                textEditorMode(&textEditorPl);
                break;
            case DRAW_BOARD:

                if (modeChanged) {
                    drawBoardPl.firstPayload = true;
                }

                drawBoardMode(&drawBoardPl);
                break;
        }

        // tell output payload is ready
        semop(semID, &v[SEM_MAIN_TO_OUTPUT], 1);
        // wait for output to complete
        semop(semID, &p[SEM_OUTPUT_TO_MAIN], 1);

        initializeSharedMemory();
        usleep(180000);

        // tell input payload is read
        semop(semID, &v[SEM_MAIN_TO_INPUT], 1);
    }

    memset(outputBuffer->inUse, false, sizeof(outputBuffer->inUse));
    // tell output payload is ready
    semop(semID, &v[SEM_MAIN_TO_OUTPUT], 1);
    // wait for output to complete
    semop(semID, &p[SEM_OUTPUT_TO_MAIN], 1);
}

void throwError(const char* error) {
    perror(error);
    exit(1);
}

void clockMode(const clockPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;

    int fnd, leds;

    // Obtain device's local time
    time_t rawtime;
    time(&rawtime);
    const struct tm* timeinfo = localtime(&rawtime);
    const int deviceSec = timeinfo->tm_sec;
    const int deviceMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;

    static int offset = 0, tmpOffset = 0;
    static bool inEdit = false;

    // Just changed into Clock mode
    if (payload->firstPayload) {
        tmpOffset = offset;
        inEdit = false;
    }

    // In normal option
    if (!inEdit) {
        // Trigger to edit option
        if (payload->toggleEdit) {
            inEdit = true;

            leds = LED_3 | LED_4;
        } else {
            leds = LED_1;
        }
    }
    // In edit option
    else {
        // Trigger to normal option
        if (payload->toggleEdit) {
            inEdit = false;
            offset = tmpOffset;

            leds = LED_1;
        } else {
            if (payload->increaseHour) {
                tmpOffset += 60;
            } else if (payload->increaseMinute) {
                tmpOffset += 1;
            } else if (payload->resetClock) {
                inEdit = false;
                offset = tmpOffset = 0;

                leds = LED_1;
            }

            // Toggle between LED(3) and LED(4)
            if ((deviceSec % 2) == 0) {
                leds = LED_3;
            } else {
                leds = LED_4;
            }
        }
        tmpOffset %= 24 * 60;
    }

    // Calculate time with offset
    const int savedHour = ((deviceMinutes + tmpOffset) / 60) % 24;
    const int savedMin = (deviceMinutes + tmpOffset) % 60;
    fnd = savedHour * 100 + savedMin;

    // Save to shared memory
    outputBuffer->ledBuffer = leds;
    outputBuffer->fndBuffer = fnd;
}

void counterMode(const counterPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;

    int fnd, leds;
    char digits[5] = {'\0'};

    static int value = 0;
    static enum _counterRadix radix = DEC;

    // Just changed into Counter mode
    if (payload->firstPayload) {
        value = 0;
        radix = DEC;
    }
    // Change to next radix
    else if (payload->changeRadix) {
        radix = (radix + 1) % COUNTER_RADIX_CNT;
    }

    // Increase least significant digit
    if (payload->increase0) {
        value++;
    }

    switch (radix) {
        case DEC:
            if (payload->increase1) {
                value += 10;
            }
            if (payload->increase2) {
                value += 100;
            }

            // Ignore 1,000's-digit for decimal
            value %= 1000;
            sprintf(digits, "%04d", value);
            leds = LED_2;
            break;
        case OCT:
            if (payload->increase1) {
                value += 010;
            }
            if (payload->increase2) {
                value += 0100;
            }

            // Reduce octal value to 4 digits
            value %= 010000;
            sprintf(digits, "%04o", value);
            leds = LED_3;
            break;
        case QUA:
            if (payload->increase1) {
                value += 0b100;
            }
            if (payload->increase2) {
                value += 0b10000;
            }

            // Reduce quaternary value to 4 digits
            value %= 0b100000000;
            digits[0] = ((value >> 6) & 0b11) + '0';
            digits[1] = ((value >> 4) & 0b11) + '0';
            digits[2] = ((value >> 2) & 0b11) + '0';
            digits[3] = ((value >> 0) & 0b11) + '0';
            leds = LED_4;
            break;
        case BIN:
            if (payload->increase1) {
                value += 0b10;
            }
            if (payload->increase2) {
                value += 0b100;
            }

            // Reduce binary value to 4 digits
            value %= 0b10000;
            digits[0] = ((value >> 3) & 1) + '0';
            digits[1] = ((value >> 2) & 1) + '0';
            digits[2] = ((value >> 1) & 1) + '0';
            digits[3] = ((value >> 0) & 1) + '0';
            leds = LED_1;
            break;
    }

    fnd = atoi(digits);

    // Save to shared memory
    outputBuffer->ledBuffer = leds;
    outputBuffer->fndBuffer = fnd;
}

void textEditorMode(const textEditorPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = true;

    int i;
    char character;
    bool moveIndex;

    static char text[TEXT_LCD_MAX_LEN] = {'\0'};
    static bool alpha = true;
    static int count = 0;
    static int index = -1;
    static int prevKey = -1;
    static int prevKeyCount = 0;

    // Just changed into Text Editor mode
    if (payload->firstPayload) {
        memset(text, ' ', TEXT_LCD_MAX_LEN);
        alpha = true;
        count = 0;
        index = -1;
        prevKey = -1;
        prevKeyCount = 0;
    }

    moveIndex = true;
    character = '\0';
    if (payload->clearText) {
        memset(text, ' ', TEXT_LCD_MAX_LEN);
        index = -1;
        count++;
        prevKey = -1;
        prevKeyCount = 0;
    } else if (payload->putSpace) {
        character = ' ';
        count++;
        prevKey = -1;
        prevKeyCount = 0;
    } else if (payload->triggerAlNum) {
        alpha = !alpha;
        count++;
        prevKey = -1;
        prevKeyCount = 0;
    } else if (alpha) {
        for (i = 1; i <= SWITCH_CNT; i++) {
            if (payload->keypad[i]) {
                if (i == prevKey) {
                    prevKeyCount = (prevKeyCount + 1) % 3;
                    moveIndex = false;
                } else {
                    prevKey = i;
                    prevKeyCount = 0;
                }
                character = TEXT_EDITOR_ALPHA[i][prevKeyCount];
                count++;
                break;
            }
        }
    } else {
        for (i = 1; i <= SWITCH_CNT; i++) {
            if (payload->keypad[i]) {
                character = '0' + i;
                count++;
                prevKey = -1;
                prevKeyCount = 0;
                break;
            }
        }
    }

    if (character != '\0') {
        if (index >= TEXT_LCD_MAX_LEN) {
            char* temp = (char*) calloc(TEXT_LCD_MAX_LEN + 1, sizeof(char));
            memcpy(temp, text + 1, TEXT_LCD_MAX_LEN - 1);
            memcpy(text, temp, TEXT_LCD_MAX_LEN - 1);
            free(temp);
            text[TEXT_LCD_MAX_LEN - 1] = character;
        } else {
            if (moveIndex) {
                index++;
            }
            text[index] = character;
        }
    }

    // Save to shared memory
    outputBuffer->fndBuffer = count;
    outputBuffer->dotCharBuffer = alpha ? 'A' : '1';
    memcpy(outputBuffer->textLcdBuffer, text, TEXT_LCD_MAX_LEN);
}

void drawBoardMode(const drawBoardPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = false;
    // int fnd;
    // char dot[DOT_ROWS][DOT_COLS];

    // Just changed into Draw Board mode
    if (payload->firstPayload) {
    }

    // Save to shared memory
    // outputBuffer->fndBuffer = fnd;
    // memcpy(outputBuffer->dotArrayBuffer, dot, sizeof(dot));
}
