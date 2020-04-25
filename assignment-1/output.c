#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *toOutput;

void output(const int semID) {
    while (true) {
        // wait until output ready
        semop(semID, &p[SEM_MAIN_READY], 1);

        write(STDOUT_FILENO, toOutput->buf, toOutput->nread);
        memset(toOutput->buf, '\0', SHM_SIZE);

        // tell output is complete
        semop(semID, &v[SEM_OUTPUT_WRITE], 1);
    }
}
