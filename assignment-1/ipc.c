#include "core.h"

extern struct _shmInBuf *inputBuffer;
extern struct _shmOutBuf *outputBuffer;

processType createForks() {
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
                default:  // parent
                    return MAIN;
            }
            break;
    }
    return MAIN;
}

int getSemaphore() {
    int semID = semget(SEM_KEY, SEM_CNT, IPC_CREAT);
    if (semID == -1) {
        throwError("Error while getting semaphore!");
    }

    union semun op;
    op.val = 0;

    int i;
    for (i = 0; i < SEM_CNT; i++) {
        if (semctl(semID, i, SETVAL, op) == -1) {
            throwError("Error while setting up semaphore!");
        }

        p[i].sem_num = v[i].sem_num = i;
        p[i].sem_flg = v[i].sem_flg = SEM_UNDO;

        p[i].sem_op = -1;
        v[i].sem_op = 1;
    }

    return semID;
}

int getSharedMemory(const key_t key, void **buf, const size_t bufferSize) {
    int shmID = shmget(key, bufferSize, IPC_CREAT);
    if (shmID == -1) {
        throwError("Error while getting shared memory!");
    }

    if ((*buf = shmat(shmID, 0, 0)) == (void *) -1) {
        throwError("Error while attaching to shared memory!");
    }

    return shmID;
}

void initializeSharedMemory() {
    inputBuffer->hasInput = false;
    inputBuffer->key = -1;
    memset(inputBuffer->switches, false, sizeof(inputBuffer->switches));

    outputBuffer->fndBuffer = 0;
    outputBuffer->ledBuffer = 0;
    outputBuffer->dotCharBuffer = ' ';
    memset(outputBuffer->inUse, false, sizeof(outputBuffer->inUse));
    memset(outputBuffer->dotArrayBuffer, false,
           sizeof(outputBuffer->dotArrayBuffer));
    memset(outputBuffer->textLcdBuffer, '\0',
           sizeof(outputBuffer->textLcdBuffer));
}
