#include "core.h"

/*
 * A logger function to use across the driver.
 */
void logger(const log_level level, const char* format, ...) {
    va_list args;

    switch (level) {
        case INFO:
            printk(KERN_INFO "[INFO] - ");
            break;
        case DEBUG:
            printk(KERN_DEBUG "[DEBUG] - ");
            break;
        case WARN:
            printk(KERN_WARNING "[WARNING] - ");
            break;
        case ERROR:
            printk(KERN_ERR "[ERROR] - ");
            break;
    }

    va_start(args, format);
    vprintk(format, args);
    va_end(args);
}
