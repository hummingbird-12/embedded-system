#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define EXIT_TIMEOUT_SECS 3

void start_switch_timer(void);
void delete_switch_timer(void);
void start_score_timer(const int);
void delete_score_timer(void);
void start_exit_timer(void);
void delete_exit_timer(void);

void initialize_timers(void);
void delete_timers_sync(void);
void delete_timers(void);

#endif /* _TIMER_H_INCLUDED_ */
