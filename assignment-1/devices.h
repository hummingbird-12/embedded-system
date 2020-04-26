#ifndef _DEVICES_H_INCLUDED_
#define _DEVICES_H_INCLUDED_

#define DEVICES_CNT 4

#define DOT_DEVICE "/dev/fpga_dot"
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/mem"
#define TEXT_LCD_DEVICE "/dev/fpga_text_lcd"

#define FND_MAX_DIGITS 4

#define FPGA_BASE_ADDRESS 0x08000000  // fpga_base address
#define LED_ADDR 0x16

enum _DEVICES { DOT, FND, LED, TEXT_LCD };
enum _LOG_LEVEL { ERROR, WARNING, INFO };

void openDevices();
void closeDevices();
void printError(enum _DEVICES, const enum _LOG_LEVEL, const char*, ...);

void fndPrint(char*);

#endif /* _DEVICES_H_INCLUDED_ */
