#ifndef _TIMER_H_INCLUDED_
#define _TIMER_H_INCLUDED_

#define STATE_ROTATION 8

#define STUDENT_ID "20161577"
#define STUDENT_NAME "Inho Kim"

typedef struct _TIMER_STATE {
    struct timer_list timer; /* Timer to register */
    char text[2][17];        /* Text to render in Text LCD device */
    enum _DIR {
        RIGHT,
        LEFT
    } text_direction[2]; /* Direction of movement for each line of text */
    int count;           /* Timer's increasing count */
    int COUNT_END;       /* Timer's total number of counts */
    int INTERVAL;        /* Timer's interval between each count */
    short text_index[2]; /* Indexes to start printing each line */
    short digit;         /* Current digit */
    short digit_index;   /* Current index to print the digit */
} t_state;

void initialize_timer_state(const char*, const int, const int);
void start_timer(void);
void initizlize_timer(void);
void delete_timer(void);

#endif /* _TIMER_H_INCLUDED_ */
