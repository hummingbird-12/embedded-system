#ifndef _FPGA_H_INCLUDED_
#define _FPGA_H_INCLUDED_

#define FPGA_DEVICES_CNT 5

#define DOT_ADDRESS 0x08000210
#define FND_ADDRESS 0x08000004
#define LED_ADDRESS 0x08000016
#define SWITCH_ADDRESS 0x08000050
#define TEXT_LCD_ADDRESS 0x08000090

#define TEXT_LCD_BUFFER_SIZE 32

typedef enum _FPGA_DEVICES { DOT, FND, LED, SWITCH, TEXT_LCD } fpga;

void fpga_iomap_devices(void);
void fpga_iounmap_devices(void);
void fpga_fnd_write(const int, const int);

#endif /* _FPGA_H_INCLUDED_ */
