#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct _shmInBuf* inputBuffer;

// The `Input` process's main loop function
void input(const int semID) {
    int switches;
    enum _keys key;
    enum _switches sw;

    while (true) {
        // Read from devices and store in the shared memory
        if ((key = keyRead()) != 0) {
            inputBuffer->key = key;
        } else if ((switches = switchRead()) != 0) {
            for (sw = SWITCH_CNT; sw >= 1; sw--) {
                inputBuffer->switches[sw] = switches % 10;
                switches /= 10;
            }
        }

        // sleep for 0.13 sec
        usleep(130000);

        // [SEMAPHORE] - let `Main` process proceed
        semop(semID, &v[SEM_INPUT_TO_MAIN], 1);
        // [SEMAPHORE] - wait for `Main` process
        semop(semID, &p[SEM_MAIN_TO_INPUT], 1);
    }
}
