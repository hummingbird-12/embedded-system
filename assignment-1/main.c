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
    bool modeChanged, hasChange, quitFlag = false;
    int i;

    clockPayload clockPl;
    counterPayload counterPl;
    textEditorPayload textEditorPl;
    drawBoardPayload drawBoardPl;

    while (true) {
        // wait for input's payload
        semop(semID, &p[SEM_INPUT_TO_MAIN], 1);

        modeChanged = false;
        hasChange = false;

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

                hasChange = clockMode(&clockPl);
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

                hasChange = counterMode(&counterPl);
                break;
            case TEXT_EDITOR:
                textEditorPl = (textEditorPayload){.firstPayload = false,
                                                   .clearText = false,
                                                   .triggerAlNum = false,
                                                   .putSpace = false};
                for (i = 0; i < SWITCH_CNT; i++) {
                    textEditorPl.keypad[i] = false;
                }

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
                for (i = 0; i < SWITCH_CNT; i++) {
                    textEditorPl.keypad[i] = inputBuffer->switches[i];
                }

                hasChange = textEditorMode(&textEditorPl);
                break;
            case DRAW_BOARD:
                drawBoardPl = (drawBoardPayload){.firstPayload = false,
                                                 .resetMode = false,
                                                 .invertDrawing = false,
                                                 .clearCanvas = false,
                                                 .toggleCursor = false,
                                                 .drawDot = false};
                for (i = 0; i < DRAW_BOARD_DIR_CNT; i++) {
                    drawBoardPl.moveCursor[i] = false;
                }

                if (modeChanged) {
                    drawBoardPl.firstPayload = true;
                }

                switch (activeInputSwitch()) {
                    case 1:
                        drawBoardPl.resetMode = true;
                        break;
                    case 2:
                        drawBoardPl.moveCursor[UP] = true;
                        break;
                    case 3:
                        drawBoardPl.toggleCursor = true;
                        break;
                    case 4:
                        drawBoardPl.moveCursor[LEFT] = true;
                        break;
                    case 5:
                        drawBoardPl.drawDot = true;
                        break;
                    case 6:
                        drawBoardPl.moveCursor[RIGHT] = true;
                        break;
                    case 7:
                        drawBoardPl.clearCanvas = true;
                        break;
                    case 8:
                        drawBoardPl.moveCursor[DOWN] = true;
                        break;
                    case 9:
                        drawBoardPl.invertDrawing = true;
                        break;
                    default:
                        break;
                }

                hasChange = drawBoardMode(&drawBoardPl);
                break;
            default:
                break;
        }

        if (hasChange) {
            // tell output payload is ready
            semop(semID, &v[SEM_MAIN_TO_OUTPUT], 1);
            // wait for output to complete
            semop(semID, &p[SEM_OUTPUT_TO_MAIN], 1);
        }

        initializeSharedMemory();

        // tell input payload is read
        semop(semID, &v[SEM_MAIN_TO_INPUT], 1);
    }

    for (i = 0; i < OUTPUT_DEVICES_CNT; i++) {
        outputBuffer->inUse[i] = false;
    }
    // tell output payload is ready
    semop(semID, &v[SEM_MAIN_TO_OUTPUT], 1);
    // wait for output to complete
    semop(semID, &p[SEM_OUTPUT_TO_MAIN], 1);
}

void throwError(const char* error) {
    perror(error);
    killChildProcesses();
    exit(1);
}

bool clockMode(const clockPayload* payload) {
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

    static int minutesBeforeEdit = 0;
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
            minutesBeforeEdit = deviceMinutes;

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
    const int offsetMinutes =
        (inEdit ? minutesBeforeEdit : deviceMinutes) + tmpOffset;
    const int savedHour = (offsetMinutes / 60) % 24;
    const int savedMin = offsetMinutes % 60;
    fnd = savedHour * 100 + savedMin;

    // Save to shared memory
    outputBuffer->ledBuffer = leds;
    outputBuffer->fndBuffer = fnd;

    return true;
}

bool counterMode(const counterPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;

    int fnd, leds;
    char digits[5] = {'\0'};
    bool hasChange = false;

    static int value = 0;
    static enum _counterRadix radix = DEC;

    // Just changed into Counter mode
    if (payload->firstPayload) {
        value = 0;
        radix = DEC;
        hasChange = true;
    }
    // Change to next radix
    else if (payload->changeRadix) {
        radix = (radix + 1) % COUNTER_RADIX_CNT;
        hasChange = true;
    }

    // Increase least significant digit
    if (payload->increase0) {
        value++;
        hasChange = true;
    }

    if (payload->increase1 || payload->increase2) {
        hasChange = true;
    }

    if (!hasChange) {
        return false;
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

    return true;
}

bool textEditorMode(const textEditorPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = true;

    int i;
    char character;
    bool moveIndex, hasChange = false;

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
        hasChange = true;
    }

    moveIndex = true;
    character = '\0';
    if (payload->clearText) {
        memset(text, ' ', TEXT_LCD_MAX_LEN);
        index = -1;
        count++;
        prevKey = -1;
        prevKeyCount = 0;
        hasChange = true;
    } else if (payload->putSpace) {
        character = ' ';
        count++;
        prevKey = -1;
        prevKeyCount = 0;
        hasChange = true;
    } else if (payload->triggerAlNum) {
        alpha = !alpha;
        count++;
        prevKey = -1;
        prevKeyCount = 0;
        hasChange = true;
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
                hasChange = true;
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
                hasChange = true;
                break;
            }
        }
    }

    if (!hasChange) {
        return false;
    }

    if (character != '\0') {
        if (index >= TEXT_LCD_MAX_LEN) {
            if (moveIndex) {
                char* temp = (char*) calloc(TEXT_LCD_MAX_LEN + 1, sizeof(char));
                memcpy(temp, text + 1, TEXT_LCD_MAX_LEN - 1);
                memcpy(text, temp, TEXT_LCD_MAX_LEN - 1);
                free(temp);
            }
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

    return true;
}

bool drawBoardMode(const drawBoardPayload* payload) {
    // Set which devices will this mode use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = false;

    const short directionsX[] = {-1, 0, 1, 0};
    const short directionsY[] = {0, 1, 0, -1};

    int newCX, newCY, i, j;
    enum _drawBoardDirections dir;
    bool hasChange = false;

    static int cursorX = 0, cursorY = 0;
    static int count = 0;
    static bool showCursor = true;
    static bool canvas[DOT_ROWS * DOT_COLS] = {false};

    // Just changed into Draw Board mode
    if (payload->firstPayload) {
        cursorX = cursorY = 0;
        count = 0;
        showCursor = true;
        drawBoardClearCanvas(canvas);
        hasChange = true;
    }

    if (payload->resetMode) {
        cursorX = cursorY = 0;
        count = 0;
        showCursor = true;
        drawBoardClearCanvas(canvas);
        hasChange = true;
    } else if (payload->clearCanvas) {
        drawBoardClearCanvas(canvas);
        count++;
        hasChange = true;
    } else if (payload->toggleCursor) {
        showCursor = !showCursor;
        count++;
        hasChange = true;
    } else if (payload->drawDot) {
        canvas[cursorX * DOT_ROWS + cursorY] =
            !canvas[cursorX * DOT_ROWS + cursorY];
        count++;
        hasChange = true;
    } else if (payload->invertDrawing) {
        for (i = 0; i < DOT_ROWS; i++) {
            for (j = 0; j < DOT_COLS; j++) {
                canvas[i * DOT_ROWS + j] = !canvas[i * DOT_ROWS + j];
            }
        }
        count++;
        hasChange = true;
    } else {
        for (dir = 0; dir < DRAW_BOARD_DIR_CNT; dir++) {
            newCX = cursorX + directionsX[dir];
            newCY = cursorY + directionsY[dir];
            if (payload->moveCursor[dir]) {
                if (newCX >= 0 && newCX < DOT_ROWS && newCY >= 0 &&
                    newCY < DOT_COLS) {
                    cursorX = newCX;
                    cursorY = newCY;
                }
                count++;
                hasChange = true;
                break;
            }
        }
    }

    // Save to shared memory
    outputBuffer->fndBuffer = count;
    outputBuffer->dotCharBuffer = '\0';

    // Copy canvas into shared memory
    for (i = 0; i < DOT_ROWS; i++) {
        for (j = 0; j < DOT_COLS; j++) {
            outputBuffer->dotArrayBuffer[i * DOT_ROWS + j] =
                canvas[i * DOT_ROWS + j];
        }
    }

    if (showCursor) {
        // Obtain device's local time
        time_t rawtime;
        time(&rawtime);
        const struct tm* timeinfo = localtime(&rawtime);
        const int deviceSec = timeinfo->tm_sec;

        // Blink cursor
        outputBuffer->dotArrayBuffer[cursorX * DOT_ROWS + cursorY] =
            (deviceSec % 2) == 0;
        hasChange = true;
    }

    return hasChange;
}

int activeInputSwitch() {
    int i;
    for (i = 1; i <= SWITCH_CNT; i++) {
        if (inputBuffer->switches[i]) {
            return i;
        }
    }
    return -1;
}

void drawBoardClearCanvas(bool* canvas) {
    int i, j;
    for (i = 0; i < DOT_ROWS; i++) {
        for (j = 0; j < DOT_COLS; j++) {
            canvas[i * DOT_ROWS + j] = false;
        }
    }
}
