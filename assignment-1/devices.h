#ifndef _DEVICES_H_INCLUDED_
#define _DEVICES_H_INCLUDED_

// Enable this Flag to show device logs
// #define _DEBUG_FLAG_

#define DEVICES_CNT 6
#define INPUT_DEVICES_CNT 2   // KEY, SWITCH
#define OUTPUT_DEVICES_CNT 4  // DOT, FND, LED, TEXT_LCD

// Device driver files
#define DOT_DEVICE "/dev/fpga_dot"
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define KEY_DEVICE "/dev/input/event0"
#define SWITCH_DEVICE "/dev/fpga_push_switch"

// Constants for Dot Matrix device
#define DOT_ROWS 10
#define DOT_COLS 7
#define DOT_0000 0x0  // ....
#define DOT_0001 0x1  // ...@
#define DOT_0010 0x2  // ..@.
#define DOT_0011 0x3  // ..@@
#define DOT_0100 0x4  // .@..
#define DOT_0101 0x5  // .@.@
#define DOT_0110 0x6  // .@@.
#define DOT_0111 0x7  // .@@@
#define DOT_1000 0x8  // @...
#define DOT_1001 0x9  // @..@
#define DOT_1010 0xA  // @.@.
#define DOT_1011 0xB  // @.@@
#define DOT_1100 0xC  // @@..
#define DOT_1101 0xD  // @@.@
#define DOT_1110 0xE  // @@@.
#define DOT_1111 0xF  // @@@@

// Constant for FND device
#define FND_MAX_DIGITS 4

// Constants for LED device
#define FPGA_BASE_ADDR 0x08000000  // fpga_base address
#define FPGA_MAP_LENGTH 4096
#define LED_ADDR_OFFSET 0x16
#define LED_1 1 << 7
#define LED_2 1 << 6
#define LED_3 1 << 5
#define LED_4 1 << 4
#define LED_5 1 << 3
#define LED_6 1 << 2
#define LED_7 1 << 1
#define LED_8 1 << 0

// Constant for Text LCD device
#define TEXT_LCD_MAX_LEN 32

// Constants for Key device
#define KEY_MAX_CNT 64
#define KEYS_CNT 4

// Constant for Switch device
#define SWITCH_CNT 9

#define BUTTON_PRESSED 1

enum _devices { DOT, FND, LED, TEXT_LCD, KEY, SWITCH };
enum _logLevel { ERROR, WARNING, INFO };
enum _keys {
    VOL_DOWN = KEY_VOLUMEDOWN,
    VOL_UP = KEY_VOLUMEUP,
    BACK = KEY_BACK,
    PROG = KEY_POWER
};
enum _switches { SW1 = 1, SW2, SW3, SW4, SW5, SW6, SW7, SW8, SW9 };

void openDevices();
void closeDevices();
void deviceLog(const enum _devices, const enum _logLevel, const char *, ...);
void writeToDevice(const enum _devices, const void *, const size_t);
void resetDevices();

void dotPrintArray(const bool *);
void dotPrintChar(const char);
void dotReset();

void fndPrint(const int);
void fndReset();

void ledPrint(const int);
void ledReset();

void textLcdPrint(const char *);
void textLcdReset();

enum _keys keyRead();
int switchRead();

#endif /* _DEVICES_H_INCLUDED_ */
