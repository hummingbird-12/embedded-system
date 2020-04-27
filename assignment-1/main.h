#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#define MODES_CNT 4

enum _mode { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD };

struct _clockPayload {
    bool firstPayload;
    bool toggleEdit;
    bool increaseHour;
    bool increaseMinute;
    bool resetClock;
};

struct _counterPayload {};

struct _textEditorPayload {};

struct _drawBoardPayload {};

void _main(const int);

void clockMode(const struct _clockPayload*);

#endif /* _MAIN_H_INCLUDED_ */
