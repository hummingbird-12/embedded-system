#include "core.h"

extern struct sembuf p[SEM_CNT], v[SEM_CNT];
extern struct shmbuf *fromInput, *toOutput;

enum _MODE { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD } mode = CLOCK;

int main() {
    int semID = getSemaphore();
    getSharedMemory(SHM_KEY_1, &fromInput);
    getSharedMemory(SHM_KEY_2, &toOutput);

    fputs("[MAIN] Starting program...\n", stdout);
    switch (createForks()) {
        case MAIN:
            while (true) {
                printf("[MAIN] waiting for INPUT\n");
                // wait until input
                semop(semID, &p[SEM_INPUT_READ], 1);
                printf("[MAIN] signalled by INPUT\n");
                memset(toOutput->buf, '\0', SHM_SIZE);
                strcpy(toOutput->buf, "Input received: ");
                strcat(toOutput->buf, fromInput->buf);
                toOutput->nread = strlen(toOutput->buf) * sizeof(char);
                printf("[MAIN] signalling OUTPUT\n");
                // tell output it's ready
                semop(semID, &v[SEM_MAIN_READY], 1);
            }
            break;
        case INPUT:
            fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
            while (true) {
                fromInput->nread =
                    read(STDIN_FILENO, fromInput->buf, sizeof(fromInput->buf));
                if (fromInput->nread > 0 && fromInput->buf[0] != '\n') {
                    printf("[INPUT] signalling MAIN\n");
                    // tell input is complete
                    semop(semID, &v[SEM_INPUT_READ], 1);
                    printf("[INPUT] waiting for OUTPUT\n");
                    // wait until output
                    semop(semID, &p[SEM_OUTPUT_WRITE], 1);
                    printf("[INPUT] signalled by OUTPUT\n");
                    memset(fromInput->buf, '\0', SHM_SIZE);
                }
            }
            break;
        case OUTPUT:
            while (true) {
                printf("[OUTPUT] waiting for MAIN\n");
                // wait until output ready
                semop(semID, &p[SEM_MAIN_READY], 1);

                printf("[OUTPUT] signalled by MAIN\n");
                write(STDOUT_FILENO, toOutput->buf, toOutput->nread);

                printf("[OUTPUT] signalling INPUT\n");
                // tell output is complete
                semop(semID, &v[SEM_OUTPUT_WRITE], 1);
            }
            break;
    }

    return 0;
}

void throwError(const char* error) {
    perror(error);
    exit(1);
}
