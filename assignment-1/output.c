#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmOutBuf* outputBuffer;

void output(const int semID) {
    int i;
    bool currentInUse[OUTPUT_DEVICES_CNT];
    memset(currentInUse, false, sizeof(currentInUse));

    resetDevices();

    while (true) {
        // [SEMAPHORE] - wait for `Main` process
        semop(semID, &p[SEM_MAIN_TO_OUTPUT], 1);

        // Check shared memory for Dot Matrix device
        if (outputBuffer->inUse[DOT]) {
            outputBuffer->dotCharBuffer == '\0'
                ? dotPrintArray(outputBuffer->dotArrayBuffer)
                : dotPrintChar(outputBuffer->dotCharBuffer);
        }
        // Only reset if was in use previously
        else if (currentInUse[DOT]) {
            dotReset();
        }

        // Check shared memory for FND device
        if (outputBuffer->inUse[FND]) {
            fndPrint(outputBuffer->fndBuffer);
        }
        // Only reset if was in use previously
        else if (currentInUse[FND]) {
            fndReset();
        }

        // Check shared memory for LED device
        if (outputBuffer->inUse[LED]) {
            ledPrint(outputBuffer->ledBuffer);
        }
        // Only reset if was in use previously
        else if (currentInUse[LED]) {
            ledReset();
        }

        // Check shared memory for Text LCD device
        if (outputBuffer->inUse[TEXT_LCD]) {
            textLcdPrint(outputBuffer->textLcdBuffer);
        }
        // Only reset if was in use previously
        else if (currentInUse[TEXT_LCD]) {
            textLcdReset();
        }

        // Save which devices are used in this loop
        for (i = 0; i < OUTPUT_DEVICES_CNT; i++) {
            currentInUse[i] = outputBuffer->inUse[i];
        }

        // [SEMAPHORE] - let `Main` process proceed
        semop(semID, &v[SEM_OUTPUT_TO_MAIN], 1);
    }
}
