/* FPGA Step motor Ioremap Control
FILE : fpga_step_motor_driver.c 
AUTH : huins.com */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>


#define IOM_FPGA_STEP_MOTOR_MAJOR 267		// ioboard led device major number
#define IOM_FPGA_STEP_MOTOR_NAME "fpga_step_motor"		// ioboard led device name

#define IOM_FPGA_STEP_MOTOR_ADDRESS 0x0800000C // pysical address


//Global variable
static int fpga_step_motor_port_usage = 0;
static unsigned char *iom_fpga_step_motor_addr;

// define functions...
ssize_t iom_fpga_step_motor_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
int iom_fpga_step_motor_open(struct inode *minode, struct file *mfile);
int iom_fpga_step_motor_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_fpga_step_motor_fops =
{
	owner:		THIS_MODULE,
	open:		iom_fpga_step_motor_open,
	write:		iom_fpga_step_motor_write,	
	release:	iom_fpga_step_motor_release,
};

// when fpga_step_motor device open ,call this function
int iom_fpga_step_motor_open(struct inode *minode, struct file *mfile) 
{	
	if(fpga_step_motor_port_usage != 0) return -EBUSY;

	fpga_step_motor_port_usage = 1;


	return 0;
}

// when fpga_step_motor device close ,call this function
int iom_fpga_step_motor_release(struct inode *minode, struct file *mfile) 
{
	fpga_step_motor_port_usage = 0;

	return 0;
}

// when write to fpga_step_motor device  ,call this function
ssize_t iom_fpga_step_motor_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) 
{
	int i;

	unsigned char value[3];
	unsigned short int _s_value = 0;
	const char *tmp = gdata;

	if (copy_from_user(&value, tmp, length))
		return -EFAULT;

    _s_value = value[0] & 0xF;
    outw(_s_value,(unsigned int)iom_fpga_step_motor_addr);
    _s_value = value[1] & 0xF;
    outw(_s_value,(unsigned int)iom_fpga_step_motor_addr + 2);
    _s_value = value[2] & 0xFF;
    outw(_s_value,(unsigned int)iom_fpga_step_motor_addr + 4);
	
	return length;
}


int __init iom_fpga_step_motor_init(void)
{
	int result;

	result = register_chrdev(IOM_FPGA_STEP_MOTOR_MAJOR, IOM_FPGA_STEP_MOTOR_NAME, &iom_fpga_step_motor_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	iom_fpga_step_motor_addr = ioremap(IOM_FPGA_STEP_MOTOR_ADDRESS, 0x4);

	printk("init module, %s major number : %d\n", IOM_FPGA_STEP_MOTOR_NAME, IOM_FPGA_STEP_MOTOR_MAJOR);

	return 0;
}

void __exit iom_fpga_step_motor_exit(void) 
{
	iounmap(iom_fpga_step_motor_addr);

	unregister_chrdev(IOM_FPGA_STEP_MOTOR_MAJOR, IOM_FPGA_STEP_MOTOR_NAME);
}

module_init(iom_fpga_step_motor_init);
module_exit(iom_fpga_step_motor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
