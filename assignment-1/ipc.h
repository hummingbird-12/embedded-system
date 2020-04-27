#ifndef _IPC_H_INCLUDED_
#define _IPC_H_INCLUDED_

#define SEM_KEY (key_t) 0x20
#define SEM_CNT 4
#define SEM_INPUT_TO_MAIN 0
#define SEM_MAIN_TO_INPUT 1
#define SEM_MAIN_TO_OUTPUT 2
#define SEM_OUTPUT_TO_MAIN 3

#define SHM_KEY_1 (key_t) 0x10
#define SHM_KEY_2 (key_t) 0x20
#define SHM_SIZE 2048

typedef enum _processType { MAIN, INPUT, OUTPUT } processType;

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

struct _shmInBuf {
    bool switches[SWITCH_CNT];
    bool keys[KEYS_CNT];
    bool hasInput;
};

struct _shmOutBuf {
    bool dotArrayBuffer[DOT_ROWS][DOT_COLS];
    char textLcdBuffer[TEXT_LCD_MAX_LEN];
    int fndBuffer;
    int ledBuffer;
    bool inUse[DEVICES_CNT];
    char dotCharBuffer;
};

struct sembuf p[SEM_CNT], v[SEM_CNT];
struct _shmInBuf *inputBuffer;
struct _shmOutBuf *outputBuffer;

processType createForks();
int getSemaphore();
int getSharedMemory(const key_t, void **, const size_t);

#endif /* _IPC_H_INCLUDED_ */
