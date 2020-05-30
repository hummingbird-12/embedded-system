#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define STATE_ROTATION 8

#define STUDENT_ID "20161577"
#define STUDENT_NAME "Inho Kim"

typedef struct _STOPWATCH_STATE {
    struct timer_list timer; /* Timer to register */
    unsigned long start_time;
    unsigned long pause_offset;
} sw_state;

void initialize_stopwatch(void);
void start_stopwatch(void);
void end_stopwatch(void);
void pause_stopwatch(void);
void reset_stopwatch(void);

void initizlize_timer(void);
void delete_timer(void);
void delete_timer_sync(void);

#endif /* _TIMER_H_INCLUDED_ */
