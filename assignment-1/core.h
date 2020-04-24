#ifndef _PRJ1_CORE_H_INCLUDED_
#define _PRJ1_CORE_H_INCLUDED_

#include <fcntl.h>  // FIXME: REMOVE LATER
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "input.h"
#include "main.h"
#include "output.h"

#define SEM_KEY (key_t)0x83

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

struct sembuf p1, p2, p3;
struct sembuf v1, v2, v3;

#endif /* _PRJ1_CORE_H_INCLUDED_ */
