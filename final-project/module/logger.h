#ifndef _LOGGER_H_INCLUDED_
#define _LOGGER_H_INCLUDED_

typedef enum _LOG_LEVEL { INFO, DEBUG, WARN, ERROR } log_level;

void logger(const log_level, const char*, ...);

#endif /* _LOGGER_H_INCLUDED_ */
