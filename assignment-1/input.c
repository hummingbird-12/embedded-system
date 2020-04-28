#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;

void input(const int semID) {
    int switches, prevSwitches = -1;
    enum _keys key, prevKey = -1;
    enum _switches sw;

    while (true) {
        inputBuffer->hasInput = false;

        if ((key = keyRead()) != 0) {
            if (key != prevKey) {
                inputBuffer->hasInput = true;
                inputBuffer->key = key;
            } else {
                inputBuffer->key = prevKey = 0;
            }
        }

        if ((switches = switchRead()) != 0) {
            if (switches != prevSwitches) {
                inputBuffer->hasInput = true;
                for (sw = SWITCH_CNT; sw >= 1; sw--) {
                    inputBuffer->switches[sw] = switches % 10;
                    switches /= 10;
                }
            } else {
                for (sw = 1; sw <= SWITCH_CNT; sw++) {
                    inputBuffer->switches[sw] = false;
                }
                prevSwitches = 0;
            }
        }

        semop(semID, &v[SEM_INPUT_TO_MAIN], 1);
        semop(semID, &p[SEM_MAIN_TO_INPUT], 1);
    }
}
