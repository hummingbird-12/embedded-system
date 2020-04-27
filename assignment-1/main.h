#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#define MODES_CNT 4

#define COUNTER_RADIX_CNT 4

enum _mode { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD };
enum _counterRadix { DEC, OCT, QUA, BIN };

typedef struct _clockPayload {
    bool firstPayload;
    bool toggleEdit;
    bool increaseHour;
    bool increaseMinute;
    bool resetClock;
} clockPayload;

typedef struct _counterPayload {
    bool firstPayload;
    bool changeRadix;
    bool increase0;
    bool increase1;
    bool increase2;
} counterPayload;

typedef struct _textEditorPayload {
    bool keypad[SWITCH_CNT + 1];
    bool firstPayload;
    bool clearText;
    bool triggerAlNum;
    bool putSpace;
} textEditorPayload;

typedef struct _drawBoardPayload {
    bool firstPayload;
} drawBoardPayload;

void _main(const int);

void clockMode(const clockPayload*);
void counterMode(const counterPayload*);
void textEditorMode(const textEditorPayload*);
void drawBoardMode(const drawBoardPayload*);

#endif /* _MAIN_H_INCLUDED_ */
