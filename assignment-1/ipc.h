#ifndef _IPC_H_INCLUDED_
#define _IPC_H_INCLUDED_

// Semaphore constants
#define SEM_KEY (key_t) 0x20
#define SEM_CNT 4
#define SEM_INPUT_TO_MAIN 0
#define SEM_MAIN_TO_INPUT 1
#define SEM_MAIN_TO_OUTPUT 2
#define SEM_OUTPUT_TO_MAIN 3

// Shared Memory constants
#define SHM_KEY_1 (key_t) 0x10
#define SHM_KEY_2 (key_t) 0x20

typedef enum _processType { MAIN, INPUT, OUTPUT } processType;

// Semaphore data structure
union semun {
    int val;                // Value for SETVAL
    struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET
    unsigned short *array;  // Array for GETALL, SETALL
    struct seminfo *__buf;  // Buffer for IPC_INFO (Linux-specific)
};

// Shared Memory between `Input` and `Main` processes
struct _shmInBuf {
    bool switches[SWITCH_CNT + 1];  // Switches SW(1)~SW(9)
    enum _keys key;                 // Keys VOL-, VOL+, BACK
};

// Shared Memory between `Main` and `Output` processes
struct _shmOutBuf {
    bool dotArrayBuffer[DOT_ROWS * DOT_COLS];  // Buffer for Dot Matrix (array)
    char textLcdBuffer[TEXT_LCD_MAX_LEN];      // Buffer for Text LCD device
    int fndBuffer;                             // Buffer for FND device
    int ledBuffer;                             // Buffer for LED device
    bool inUse[OUTPUT_DEVICES_CNT];            // Devices in use
    char dotCharBuffer;                        // Buffer for Dot Matrix (char)
};

struct sembuf p[SEM_CNT], v[SEM_CNT];
struct _shmInBuf *inputBuffer;
struct _shmOutBuf *outputBuffer;

processType createForks();
int getSemaphore();
int getSharedMemory(const key_t, void **, const size_t);
void initializeSharedMemory();
void removeSemaphores(const int);
void removeSharedMemories(const int, const int);
void removeIpcObjects(const int, const int, const int);
void killChildProcesses();

#endif /* _IPC_H_INCLUDED_ */
