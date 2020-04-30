#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;

void input(const int semID) {
    int switches;
    enum _keys key;
    enum _switches sw;

    while (true) {
        if ((key = keyRead()) != 0) {
            inputBuffer->key = key;
        }

        if ((switches = switchRead()) != 0) {
            for (sw = SWITCH_CNT; sw >= 1; sw--) {
                inputBuffer->switches[sw] = switches % 10;
                switches /= 10;
            }
        }

        usleep(140000);

        semop(semID, &v[SEM_INPUT_TO_MAIN], 1);
        semop(semID, &p[SEM_MAIN_TO_INPUT], 1);
    }
}
