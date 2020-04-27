#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#define MODES_CNT 4

struct _clockPayload {
    bool modeChanged;
    bool increaseHour;
    bool increaseMinute;
    bool resetClock;
};

enum _mode { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD };

void _main(const int);

void clockMode(const struct _clockPayload*);

#endif /* _MAIN_H_INCLUDED_ */
