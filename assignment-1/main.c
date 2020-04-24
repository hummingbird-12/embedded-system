#include "core.h"

enum _MODE { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD } mode = CLOCK;

void throwError(const char* error) {
    perror(error);
    exit(1);
}

int prepareSemaphore() {
    int semID;
    if ((semID = semget(SEM_KEY, 3, IPC_CREAT)) == -1) {
        throwError("Error while getting semaphore!");
    }

    union semun op;
    op.val = 0;
    if (semctl(semID, 0, SETVAL, op) == -1) {
        throwError("Error while semaphore operation!");
    }
    if (semctl(semID, 1, SETVAL, op) == -1) {
        throwError("Error while semaphore operation!");
    }
    if (semctl(semID, 2, SETVAL, op) == -1) {
        throwError("Error while semaphore operation!");
    }

    p1.sem_num = 0;
    p1.sem_op = -1;
    p1.sem_flg = SEM_UNDO;

    v1.sem_num = 0;
    v1.sem_op = 1;
    v1.sem_flg = SEM_UNDO;

    p2.sem_num = 1;
    p2.sem_op = -1;
    p2.sem_flg = SEM_UNDO;

    v2.sem_num = 1;
    v2.sem_op = 1;
    v2.sem_flg = SEM_UNDO;

    p3.sem_num = 2;
    p3.sem_op = -1;
    p3.sem_flg = SEM_UNDO;

    v3.sem_num = 2;
    v3.sem_op = 1;
    v3.sem_flg = SEM_UNDO;

    return semID;
}

int main() {
    char buf[10] = {'\0'};
    char buf2;
    // fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    fputs("[MAIN] Starting program...\n", stdout);

    int semID = prepareSemaphore();
    switch (makeForks(semID, buf)) {
        case MAIN:
            while (true) {
                printf("[MAIN] waiting for INPUT\n");
                fflush(stdout);
                semop(semID, &p1, 1);  // wait until input
                printf("[MAIN] signalled by INPUT\n");
                fflush(stdout);
                printf("[MAIN] signalling OUTPUT\n");
                fflush(stdout);
                semop(semID, &v2, 1);  // tell output it's ready
            }
            break;
        case INPUT:
            while (true) {
                // ssize_t n = read(STDIN_FILENO, buf, 10);
                ssize_t n = read(STDIN_FILENO, &buf2, 1);
                if (n > 0) {
                    printf("[INPUT] signalling MAIN\n");
                    fflush(stdout);
                    semop(semID, &v1, 1);  // tell input is complete
                    printf("[INPUT] waiting for OUTPUT\n");
                    fflush(stdout);
                    semop(semID, &p3, 1);  // wait until output
                    printf("[INPUT] signalled by OUTPUT\n");
                    fflush(stdout);
                }
            }
            break;
        case OUTPUT:
            while (true) {
                printf("[OUTPUT] waiting for MAIN\n");
                fflush(stdout);
                semop(semID, &p2, 1);  // wait until output ready
                printf("[OUTPUT] signalled by MAIN\n");
                fflush(stdout);
                // write(STDOUT_FILENO, buf, 10);
                // write(STDOUT_FILENO, &buf2, 1);
                printf(">>> %c\n", buf2);
                fflush(stdout);
                printf("[OUTPUT] signalling INPUT\n");
                fflush(stdout);
                semop(semID, &v3, 1);  // tell output is complete
            }
            break;
    }

    return 0;
}

Process makeForks(int semID, char* buf) {
    pid_t pid;
    switch ((pid = fork())) {
        case -1:
            throwError("Error while creating child process!");
            break;
        case 0:  // child
            // input(semID, buf);
            return INPUT;
        default:  // parent
            switch ((pid = fork())) {
                case -1:
                    throwError("Error while creating child process!");
                    break;
                case 0:  // child
                    // output(semID, buf);
                    return OUTPUT;
                default:
                    return MAIN;
            }
            break;
    }
}
