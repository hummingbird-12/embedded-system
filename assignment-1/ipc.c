#include "core.h"

int getMessageQueue() {
    int msgID = msgget(MSG_KEY, IPC_CREAT);
    if (msgID == -1) {
        throwError("Error while getting message queue!");
    }

    return msgID;
}

int getSemaphore() {
    int semID = semget(SEM_KEY, SEM_CNT, IPC_CREAT);
    if (semID == -1) {
        throwError("Error while getting semaphore!");
    }

    union semun op;
    op.val = 0;

    for (int i = 0; i < SEM_CNT; i++) {
        if (semctl(semID, i, SETVAL, op) == -1) {
            throwError("Error while setting up semaphore!");
        }

        p[i].sem_op = -1;
        v[i].sem_op = 1;

        p[i].sem_num = v[i].sem_num = i;
        p[i].sem_flg = v[i].sem_flg = SEM_UNDO;
    }

    return semID;
}
