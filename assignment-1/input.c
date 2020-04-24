#include "core.h"

extern struct sembuf p1;
extern struct sembuf p2;
extern struct sembuf p3;
extern struct sembuf v1;
extern struct sembuf v2;
extern struct sembuf v3;

void input(const int semID, char* buf) {
    fputs("[INPUT] Start of input process\n", stdout);

    while (true) {
        fputs("[INPUT] Enter input: ", stdout);
        read(STDIN_FILENO, buf, 1);

        fputs("[INPUT] Beginning of CS\n", stdout);
        semop(semID, &v1, 1);
        semop(semID, &p2, 1);
    }
}
