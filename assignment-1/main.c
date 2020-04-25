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
    fputs("[MAIN] Starting program...\n", stdout);

    char buf[10] = {'\0'};
    FILE* mem;
    int semID = prepareSemaphore();
    switch (makeForks(semID)) {
        case MAIN:
            while (true) {
                printf("[MAIN] waiting for INPUT\n");
                semop(semID, &p1, 1);  // wait until input
                printf("[MAIN] signalled by INPUT\n");
                printf("[MAIN] signalling OUTPUT\n");
                semop(semID, &v2, 1);  // tell output it's ready
            }
            break;
        case INPUT:
            fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
            while (true) {
                ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
                if (n > 0 && buf[0] != '\n') {
                    if ((mem = fopen("memory.txt", "w")) == NULL) {
                        throwError("Error while opening shared memory!");
                    }
                    fputs(buf, mem);
                    fclose(mem);

                    printf("[INPUT] signalling MAIN\n");
                    semop(semID, &v1, 1);  // tell input is complete
                    printf("[INPUT] waiting for OUTPUT\n");
                    semop(semID, &p3, 1);  // wait until output
                    printf("[INPUT] signalled by OUTPUT\n");
                }
            }
            break;
        case OUTPUT:
            while (true) {
                printf("[OUTPUT] waiting for MAIN\n");
                semop(semID, &p2, 1);  // wait until output ready

                printf("[OUTPUT] signalled by MAIN\n");
                if ((mem = fopen("memory.txt", "r")) == NULL) {
                    throwError("Error while opening shared memory!");
                }
                fgets(buf, sizeof(buf), mem);
                fclose(mem);
                char out[20] = ">>> ";
                strcat(out, buf);
                write(STDOUT_FILENO, out, sizeof(out));

                printf("[OUTPUT] signalling INPUT\n");
                semop(semID, &v3, 1);  // tell output is complete
            }
            break;
    }

    return 0;
}

Process makeForks(int semID) {
    pid_t pid;
    switch ((pid = fork())) {
        case -1:
            throwError("Error while creating child process!");
            break;
        case 0:  // child
            return INPUT;
        default:  // parent
            switch ((pid = fork())) {
                case -1:
                    throwError("Error while creating child process!");
                    break;
                case 0:  // child
                    return OUTPUT;
                default:
                    return MAIN;
            }
            break;
    }
}
