#ifndef _DRIVER_SPECS_H_INCLUDED_
#define _DRIVER_SPECS_H_INCLUDED_

#include <linux/ioctl.h>

#define DEVICE_NAME "hangman"
#define DEVICE_MAJOR_NUMBER 242

#define IOCTL_READ_LETTER _IO(DEVICE_MAJOR_NUMBER, 0)

#endif /* _DRIVER_SPECS_H_INCLUDED_ */
