#ifndef _FPGA_H_INCLUDED_
#define _FPGA_H_INCLUDED_

#define FND_ADDRESS 0x08000004

void fpga_iomap_fnd(void);
void fpga_iounmap_fnd(void);
void fpga_fnd_write(const int, const int);

#endif /* _FPGA_H_INCLUDED_ */
