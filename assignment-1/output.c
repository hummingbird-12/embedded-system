#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmOutBuf* outputBuffer;

void output(const int semID) {
    bool inUse[OUTPUT_DEVICES_CNT];
    memset(inUse, false, sizeof(inUse));

    while (true) {
        // wait for main's payload
        semop(semID, &p[SEM_MAIN_TO_OUTPUT], 1);

        enum _devices i;
        for (i = 0; i < DEVICES_CNT; i++) {
            switch (i) {
                case DOT:
                    if (outputBuffer->inUse[i]) {
                        outputBuffer->dotCharBuffer == '\0'
                            ? dotPrintArray(
                                  (const bool**) outputBuffer->dotArrayBuffer)
                            : dotPrintChar(outputBuffer->dotCharBuffer);
                    } else if (inUse[i]) {
                        dotReset();
                    }
                    break;
                case FND:
                    if (outputBuffer->inUse[i]) {
                        fndPrint(outputBuffer->fndBuffer);
                    } else if (inUse[i]) {
                        fndReset();
                    }
                    break;
                case LED:
                    if (outputBuffer->inUse[i]) {
                        ledPrint(outputBuffer->ledBuffer);
                    } else if (inUse[i]) {
                        ledReset();
                    }
                    break;
                case TEXT_LCD:
                    if (outputBuffer->inUse[i]) {
                        textLcdPrint(outputBuffer->textLcdBuffer);
                    } else if (inUse[i]) {
                        textLcdReset();
                    }
                    break;
                default:
                    break;
            }
        }

        // tell main output is complete
        semop(semID, &v[SEM_OUTPUT_TO_MAIN], 1);
    }
}
