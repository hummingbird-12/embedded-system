#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmOutBuf* outputBuffer;

void output(const int semID) {
    bool currentInUse[OUTPUT_DEVICES_CNT];
    memset(currentInUse, false, sizeof(currentInUse));

    while (true) {
        // wait for main's payload
        semop(semID, &p[SEM_MAIN_TO_OUTPUT], 1);

        if (outputBuffer->inUse[DOT]) {
            outputBuffer->dotCharBuffer == '\0'
                ? dotPrintArray((const bool**) outputBuffer->dotArrayBuffer)
                : dotPrintChar(outputBuffer->dotCharBuffer);
        } else if (currentInUse[DOT]) {
            dotReset();
        }

        if (outputBuffer->inUse[FND]) {
            fndPrint(outputBuffer->fndBuffer);
        } else if (currentInUse[FND]) {
            fndReset();
        }

        if (outputBuffer->inUse[LED]) {
            ledPrint(outputBuffer->ledBuffer);
        } else if (currentInUse[LED]) {
            ledReset();
        }

        if (outputBuffer->inUse[TEXT_LCD]) {
            textLcdPrint(outputBuffer->textLcdBuffer);
        } else if (currentInUse[TEXT_LCD]) {
            textLcdReset();
        }

        memcpy(currentInUse, outputBuffer->inUse, sizeof(currentInUse));

        // tell main output is complete
        semop(semID, &v[SEM_OUTPUT_TO_MAIN], 1);
    }
}
