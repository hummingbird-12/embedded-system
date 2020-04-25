#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *toOutput;

void output(const int semID) {
    while (true) {
        write(STDOUT_FILENO, toOutput->buf, toOutput->nread);
        memset(toOutput->buf, '\0', SHM_SIZE);
        // sleep(1);
        // tell output is complete
        semop(semID, &v[SEM_MAIN_READY], 1);
    }
}
