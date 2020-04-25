#ifndef _IPC_H_INCLUDED_
#define _IPC_H_INCLUDED_

#define SEM_KEY (key_t) 0x10
#define SEM_CNT 3
#define SEM_INPUT_READY 0
#define SEM_MAIN_READY 1
#define SEM_OUTPUT_WRITE 2

#define SHM_KEY_1 (key_t) 0x20
#define SHM_KEY_2 (key_t) 0x30
#define SHM_SIZE 2048

typedef enum _PROCESS { MAIN, INPUT, OUTPUT } ProcessType;

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

struct shmbuf {
    int nread;
    char buf[SHM_SIZE];
};

struct sembuf p[SEM_CNT], v[SEM_CNT];
struct shmbuf *fromInput, *toOutput;

ProcessType createForks();
int getSemaphore();
int getSharedMemory(const key_t, struct shmbuf **);

#endif /* _IPC_H_INCLUDED_ */
