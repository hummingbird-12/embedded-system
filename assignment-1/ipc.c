#include "core.h"

extern struct _shmInBuf *inputBuffer;
extern struct _shmOutBuf *outputBuffer;

static pid_t inputPID;
static pid_t outputPID;

processType createForks() {
    pid_t pid;
    switch ((pid = fork())) {  // First fork()
        case -1:
            throwError("Error while creating child process!");
            break;
        case 0:  // child
            return INPUT;
        default:  // parent
            inputPID = pid;
            switch ((pid = fork())) {  // Second fork();
                case -1:
                    throwError("Error while creating child process!");
                    break;
                case 0:  // child
                    return OUTPUT;
                default:  // parent
                    outputPID = pid;
                    return MAIN;
            }
            break;
    }
    return MAIN;
}

int getSemaphore() {
    // Allocate the semaphone
    int semID = semget(SEM_KEY, SEM_CNT, IPC_CREAT);
    if (semID == -1) {
        throwError("Error while getting semaphore!");
    }

    union semun op;
    op.val = 0;

    // Initialize each of the semaphores
    int i;
    for (i = 0; i < SEM_CNT; i++) {
        if (semctl(semID, i, SETVAL, op) == -1) {
            throwError("Error while setting up semaphore!");
        }

        p[i].sem_num = v[i].sem_num = i;
        p[i].sem_flg = v[i].sem_flg = SEM_UNDO;

        p[i].sem_op = -1;  // The semaphore p() operation
        v[i].sem_op = 1;   // The semaphore v() operation
    }

    return semID;
}

int getSharedMemory(const key_t key, void **buf, const size_t bufferSize) {
    // Allocate the shared memory
    int shmID = shmget(key, bufferSize, IPC_CREAT);
    if (shmID == -1) {
        throwError("Error while getting shared memory!");
    }

    // Attach a buffer to the shared memory.
    if ((*buf = shmat(shmID, 0, 0)) == (void *) -1) {
        throwError("Error while attaching to shared memory!");
    }

    return shmID;
}

void initializeSharedMemory() {
    inputBuffer->key = -1;
    memset(inputBuffer->switches, false, sizeof(inputBuffer->switches));

    outputBuffer->fndBuffer = 0;
    outputBuffer->ledBuffer = 0;
    outputBuffer->dotCharBuffer = '\0';
    memset(outputBuffer->inUse, false, sizeof(outputBuffer->inUse));
    memset(outputBuffer->dotArrayBuffer, false,
           sizeof(outputBuffer->dotArrayBuffer));
    memset(outputBuffer->textLcdBuffer, '\0',
           sizeof(outputBuffer->textLcdBuffer));
}

// Deallocate semaphores
void removeSemaphores(const int semID) {
    if (semctl(semID, 0, IPC_RMID, 0) == -1) {
        throwError("Error while removing semaphore!");
    }
}

// Deallocate shared memory
void removeSharedMemories(const int shmInID, const int shmOutID) {
    if (shmctl(shmInID, IPC_RMID, 0) == -1) {
        throwError("Error while removing shared memory!");
    }
    if (shmctl(shmOutID, IPC_RMID, 0) == -1) {
        throwError("Error while removing shared memory!");
    }
}

// Deallocate all IPC objects
void removeIpcObjects(const int semID, const int shmInID, const int shmOutID) {
    removeSemaphores(semID);
    removeSharedMemories(shmInID, shmOutID);
}

// Kill child processes
void killChildProcesses() {
    static bool called = false;
    if (called) {
        perror("Child processes already killed!");
        return;
    }

    // Kill `Input` process
    if (kill(inputPID, SIGKILL) == -1) {
        perror("Error while killing input process!");
    } else {
        waitpid(inputPID, NULL, 0);
    }

    // Kill `Output` process
    if (kill(outputPID, SIGKILL) == -1) {
        perror("Error while killing output process!");
    } else {
        waitpid(outputPID, NULL, 0);
    }
    called = true;
}
