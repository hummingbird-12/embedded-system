#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define MAX_SECONDS 3600
#define EXIT_TIMEOUT_SECS 3

typedef struct _STOPWATCH_STATE {
    struct timer_list timer;     /* Timer to register */
    unsigned long last_callback; /* Jiffies value of last timer expire */
    unsigned long pause_offset;  /* Jiffies offset when stopwatch paused */
    int elapsed_seconds;         /* Tracks seconds of the stopwatch */
} sw_state;

void initialize_stopwatch(void);
void start_stopwatch(void);
void end_stopwatch(void);
void pause_stopwatch(void);
void reset_stopwatch(void);
void delete_stopwatch_timer(void);

void start_exit_timer(void);
void delete_exit_timer(void);

void initizlize_timers(void);
void delete_timers_sync(void);

#endif /* _TIMER_H_INCLUDED_ */
