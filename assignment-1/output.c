#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmOutBuf* outputBuffer;

void output(const int semID) {
    while (true) {
        // wait for main process's payload
        semop(semID, &p[SEM_MAIN_TO_OUTPUT], 1);

        enum _DEVICES i;
        for (i = 0; i < DEVICES_CNT; i++) {
            switch (i) {
                case DOT:
                    outputBuffer->inUse[i] ? dotReset() : dotReset();
                    break;
                case FND:
                    outputBuffer->inUse[i] ? fndPrint(outputBuffer->fndBuffer)
                                           : fndReset();
                    break;
                case LED:
                    outputBuffer->inUse[i] ? ledPrint(outputBuffer->ledBuffer)
                                           : ledReset();
                    break;
                case TEXT_LCD:
                    outputBuffer->inUse[i] ? textLcdReset() : textLcdReset();
                    break;
            }
        }

        // tell output print is complete
        semop(semID, &v[SEM_OUTPUT_TO_MAIN], 1);
    }
}
