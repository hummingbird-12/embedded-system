#ifndef _PRJ1_CORE_H_INCLUDED_
#define _PRJ1_CORE_H_INCLUDED_

#include <fcntl.h>  // FIXME: REMOVE LATER
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // FIXME: REMOVE LATER
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "input.h"
#include "ipc.h"
#include "main.h"
#include "output.h"

#define MSG_KEY (key_t) 0x10
#define SEM_KEY (key_t) 0x10
#define SEM_CNT 3

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

struct sembuf p[SEM_CNT], v[SEM_CNT];
struct sembuf p1, p2, p3;
struct sembuf v1, v2, v3;

#endif /* _PRJ1_CORE_H_INCLUDED_ */
