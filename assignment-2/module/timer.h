#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define STATE_ROTATION 8

#define STUDENT_ID "20161577"
#define STUDENT_NAME "Inho Kim"

typedef struct _TIMER_STATE {
    struct timer_list timer;

    char text[2][17];
    enum _DIR { RIGHT, LEFT } text_direction[2];
    int count;
    int COUNT_END;
    int INTERVAL;
    short text_index[2];
    short digit;
    short digit_index;
} t_state;

void initialize_timer(const char*, const int, const int);
void start_timer(void);
void delete_timer(void);
void timer_callback(unsigned long);
void print_state(const t_state*);
void move_text(t_state*, const int);

#endif /* _TIMER_H_INCLUDED_ */
