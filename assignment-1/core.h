#ifndef _CORE_H_INCLUDED_
#define _CORE_H_INCLUDED_

#include <ctype.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "devices.h"
#include "input.h"
#include "ipc.h"
#include "main.h"
#include "output.h"

void throwError(const char*);

#endif /* _CORE_H_INCLUDED_ */
