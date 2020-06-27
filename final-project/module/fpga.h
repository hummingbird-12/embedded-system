#ifndef _FPGA_H_INCLUDED_
#define _FPGA_H_INCLUDED_

#define FPGA_DEVICES_CNT 5

#define DOT_ADDRESS 0x08000210
#define FND_ADDRESS 0x08000004
#define LED_ADDRESS 0x08000016
#define SWITCH_ADDRESS 0x08000050
#define TEXT_LCD_ADDRESS 0x08000090

#define SWITCH_CNT 9
#define TEXT_LCD_BUFFER_SIZE 32

typedef enum _FPGA_DEVICES { DOT, FND, LED, SWITCH, TEXT_LCD } fpga;

void fpga_iomap_devices(void);
void fpga_iounmap_devices(void);
void fpga_initialize(void);

void fpga_dot_write(const char);
void fpga_fnd_write(const int);
void fpga_led_write(const int);
int fpga_switch_read(void);
void fpga_text_lcd_write(const char*);

#endif /* _FPGA_H_INCLUDED_ */
