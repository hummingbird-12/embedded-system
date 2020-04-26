#ifndef _DEVICES_H_INCLUDED_
#define _DEVICES_H_INCLUDED_

#define _DEBUG_FLAG_
#define _TESTING_FLAG_

#define DEVICES_CNT 4

#define DOT_DEVICE "/dev/fpga_dot"
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define TEXT_LCD_DEVICE "/dev/fpga_text_lcd"

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

#define FND_MAX_DIGITS 4

#define FPGA_BASE_ADDR 0x08000000  // fpga_base address
#define FPGA_MAP_LENGTH 4096
#define LED_ADDR_OFFSET 0x16

enum _DEVICES { DOT, FND, LED, TEXT_LCD };
enum _LOG_LEVEL { ERROR, WARNING, INFO };

void openDevices();
void closeDevices();
void deviceLog(const enum _DEVICES, const enum _LOG_LEVEL, const char*, ...);

void dotPrint(const char);
void fndPrint(const int);
void ledPrint(const int);

#endif /* _DEVICES_H_INCLUDED_ */
