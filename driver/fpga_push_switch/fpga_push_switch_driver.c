/* FPGA PUSH SWITCH Ioremap Control
FILE : fpga_push_switch_driver.c 
AUTH : largest@huins.com*/

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

#define MAX_BUTTON 9

#define IOM_FPGA_PUSH_SWITCH_MAJOR 265				// ioboard led device major number
#define IOM_FPGA_PUSH_SWITCH_NAME "fpga_push_switch"	// ioboard led device name

#define IOM_FPGA_PUSH_SWITCH_ADDRESS 0x08000050			// pysical address


//Global variable
static int fpga_push_switch_port_usage = 0;
static unsigned char *iom_fpga_push_switch_addr;
static unsigned char *iom_demo_addr;

// define functions...
ssize_t iom_fpga_push_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what); 
int iom_fpga_push_switch_open(struct inode *minode, struct file *mfile);
int iom_fpga_push_switch_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_fpga_push_switch_fops =
{
	owner:		THIS_MODULE,
	open:		iom_fpga_push_switch_open,
	read:		iom_fpga_push_switch_read,	
	release:	iom_fpga_push_switch_release,
};

// when fpga_push_switch device open ,call this function
int iom_fpga_push_switch_open(struct inode *minode, struct file *mfile) 
{	
	if(fpga_push_switch_port_usage != 0) return -EBUSY;

	fpga_push_switch_port_usage = 1;


	return 0;
}

// when fpga_push_switch device close ,call this function
int iom_fpga_push_switch_release(struct inode *minode, struct file *mfile) 
{
	fpga_push_switch_port_usage = 0;

	return 0;
}



// when read from fpga_push_switch device  ,call this function
ssize_t iom_fpga_push_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) 
{
	int i;
	unsigned char push_sw_value[MAX_BUTTON];	
    unsigned short int _s_value;

	for(i=0;i<length;i++) 
    {
		_s_value = inw((unsigned int)iom_fpga_push_switch_addr+i*2);
        push_sw_value[i] = _s_value &0xFF;
    }

	if (copy_to_user(gdata, &push_sw_value, length))
		return -EFAULT;

	return length;	
}

int __init iom_fpga_push_switch_init(void)
{
	int result;

	result = register_chrdev(IOM_FPGA_PUSH_SWITCH_MAJOR, IOM_FPGA_PUSH_SWITCH_NAME, &iom_fpga_push_switch_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	iom_fpga_push_switch_addr = ioremap(IOM_FPGA_PUSH_SWITCH_ADDRESS, 0x18);

	printk("init module, %s major number : %d\n", IOM_FPGA_PUSH_SWITCH_NAME, IOM_FPGA_PUSH_SWITCH_MAJOR);

	return 0;
}

void __exit iom_fpga_push_switch_exit(void) 
{
	iounmap(iom_fpga_push_switch_addr);

	unregister_chrdev(IOM_FPGA_PUSH_SWITCH_MAJOR, IOM_FPGA_PUSH_SWITCH_NAME);
}

module_init(iom_fpga_push_switch_init);
module_exit(iom_fpga_push_switch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
