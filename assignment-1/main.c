#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;
extern struct _shmOutBuf* outputBuffer;

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

    struct _clockPayload clockPayload;

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
                clockPayload.firstPayload = false;
                clockPayload.increaseHour = false;
                clockPayload.increaseMinute = false;
                clockPayload.toggleEdit = false;
                clockPayload.resetClock = false;

                if (modeChanged) {
                    clockPayload.firstPayload = true;
                }

                if (inputBuffer->switches[1]) {
                    clockPayload.toggleEdit = true;
                } else if (inputBuffer->switches[2]) {
                    clockPayload.resetClock = true;
                } else if (inputBuffer->switches[3]) {
                    clockPayload.increaseHour = true;
                } else if (inputBuffer->switches[4]) {
                    clockPayload.increaseMinute = true;
                }

                clockMode(&clockPayload);
                break;
            case COUNTER:
                break;
            case TEXT_EDITOR:
                break;
            case DRAW_BOARD:
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

void clockMode(const struct _clockPayload* payload) {
    // Set which devices this mode will use
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;

    // Obtain device's local time
    time_t rawtime;
    time(&rawtime);
    const struct tm* timeinfo = localtime(&rawtime);
    const int deviceSec = timeinfo->tm_sec;
    const int deviceMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;

    static int offset = 0, tmpOffset = 0;
    static bool inEdit = false;

    int fnd, leds;

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

void counterMode(const struct _counterPayload* payload) {
    // Set which devices this mode will use
    outputBuffer->inUse[DOT] = false;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = true;
    outputBuffer->inUse[TEXT_LCD] = false;
}

void textEditorMode(const struct _textEditorPayload* payload) {
    // Set which devices this mode will use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = true;
}

void drawBoardMode(const struct _drawBoardPayload* payload) {
    // Set which devices this mode will use
    outputBuffer->inUse[DOT] = true;
    outputBuffer->inUse[FND] = true;
    outputBuffer->inUse[LED] = false;
    outputBuffer->inUse[TEXT_LCD] = false;
}
