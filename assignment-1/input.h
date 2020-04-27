#ifndef _INPUT_H_INCLUDED_
#define _INPUT_H_INCLUDED_

#define BUTTONS_CNT 13
#define BUTTON_PRESSED 1

#define KEY_MAX_CNT 64
#define KEYS_CNT 4

#define SWITCH_CNT 9

enum _BUTTONS {
    SW1 = (1 << 0),
    SW2 = (1 << 1),
    SW3 = (1 << 2),
    SW4 = (1 << 3),
    SW5 = (1 << 4),
    SW6 = (1 << 5),
    SW7 = (1 << 6),
    SW8 = (1 << 7),
    SW9 = (1 << 8),
    PROG = (1 << 9),
    VOL_UP = (1 << 10),
    VOL_DOWN = (1 << 11),
    BACK = (1 << 12)
};

void input(const int);
void readKeys(const int, const int);
void readSwitches(const int, const int);
void writeToSHM(const int, const int);

#endif /* _INPUT_H_INCLUDED_ */
