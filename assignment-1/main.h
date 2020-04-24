#ifndef _PRJ1_MAIN_H_INCLUDED_
#define _PRJ1_MAIN_H_INCLUDED_

typedef enum _PROCESS { MAIN, INPUT, OUTPUT } Process;

Process makeForks(int, char*);
int prepareSemaphore();
void throwError(const char*);

#endif /* _PRJ1_MAIN_H_INCLUDED_ */
