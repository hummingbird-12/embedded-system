#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#define MODES_CNT 4
#define COUNTER_RADIX_CNT 4
#define DRAW_BOARD_DIR_CNT 4

enum _mode { CLOCK, COUNTER, TEXT_EDITOR, DRAW_BOARD };
enum _counterRadix { DEC, OCT, QUA, BIN };
enum _drawBoardDirections { UP, RIGHT, DOWN, LEFT };

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
    bool moveCursor[DRAW_BOARD_DIR_CNT];
    bool firstPayload;
    bool resetMode;
    bool invertDrawing;
    bool clearCanvas;
    bool toggleCursor;
    bool drawDot;
} drawBoardPayload;

void _main(const int);

bool clockMode(const clockPayload*);
bool counterMode(const counterPayload*);
bool textEditorMode(const textEditorPayload*);
bool drawBoardMode(const drawBoardPayload*);

int activeInputSwitch();
void drawBoardClearCanvas(bool*);

#endif /* _MAIN_H_INCLUDED_ */
