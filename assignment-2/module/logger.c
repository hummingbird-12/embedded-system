#include "core.h"

void logger(const log_level level, const char* format, ...) {
    va_list args;

    switch (level) {
        case INFO:
            printk(KERN_INFO "[INFO]\t- ");
            break;
        case DEBUG:
            printk(KERN_DEBUG "[DEBUG]\t- ");
            break;
        case WARN:
            printk(KERN_WARNING "[WARNING]\t- ");
            break;
        case ERROR:
            printk(KERN_ERR "[ERROR]\t- ");
            break;
    }

    va_start(args, format);
    vprintk(format, args);
    va_end(args);
}
