#ifndef _FPGA_H_INCLUDED_
#define _FPGA_H_INCLUDED_

#define FPGA_DEVICES_CNT 4

#define DOT_ADDRESS 0x08000210
#define FND_ADDRESS 0x08000004
#define LED_ADDRESS 0x08000016
#define TEXT_LCD_ADDRESS 0x08000090

typedef enum _FPGA_DEVICES { DOT, FND, LED, TEXT_LCD } fpga;

extern char *fpga_addr[FPGA_DEVICES_CNT];

void fpga_iomap_devices(void);
void fpga_iounmap_devices(void);
void fpga_dot_write(const int);
void fpga_fnd_write(const int, const int);
void fpga_led_write(const int);

#endif /* _FPGA_H_INCLUDED_ */
