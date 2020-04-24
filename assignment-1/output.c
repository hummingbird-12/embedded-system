#include "core.h"

extern struct sembuf p1;
extern struct sembuf p2;
extern struct sembuf p3;
extern struct sembuf v1;
extern struct sembuf v2;
extern struct sembuf v3;

void output(const int semID, char* buf) {
    fputs("[OUTPUT] Start of output process\n", stdout);

    while (true) {
        fputs("[OUTPUT] Beginning of CS\n", stdout);
        semop(semID, &p1, 1);
        semop(semID, &v2, 1);

        fputs("[OUTPUT] Here is the output: ", stdout);
        fputs(buf, stdout);
    }
}
