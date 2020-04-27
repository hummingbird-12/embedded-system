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
    bool quitFlag = false;

    struct _clockPayload clockPayload;

    while (true) {
        // wait for input's payload
        semop(semID, &p[SEM_INPUT_TO_MAIN], 1);

        if (inputBuffer->hasInput) {
            switch (inputBuffer->key) {
                case KEY_VOLUMEDOWN:
                    break;
                case KEY_VOLUMEUP:
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
                clockPayload.increaseHour = false;
                clockPayload.increaseMinute = false;
                clockPayload.modeChanged = false;
                clockPayload.resetClock = false;

                if (inputBuffer->switches[1]) {
                    clockPayload.modeChanged = true;
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
}
