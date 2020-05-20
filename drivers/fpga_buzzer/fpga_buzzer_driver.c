/* FPGA Buzzer Ioremap Control
FILE : fpga_buzzer_driver.c 
AUTH : largest@huins.com */

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


#define IOM_BUZZER_MAJOR 264		// ioboard buzzer device major number
#define IOM_BUZZER_NAME "fpga_buzzer"		// ioboard buzzer device name

#define IOM_BUZZER_ADDRESS 0x08000070 // pysical address

//Global variable
static int buzzer_port_usage = 0;
static unsigned char *iom_fpga_buzzer_addr;
static unsigned char *iom_demo_addr;

// define functions...
ssize_t iom_buzzer_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
ssize_t iom_buzzer_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
int iom_buzzer_open(struct inode *minode, struct file *mfile);
int iom_buzzer_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_buzzer_fops =
{
	.owner		=	THIS_MODULE,
	.open		=	iom_buzzer_open,
	.write		=	iom_buzzer_write,	
	.read		=	iom_buzzer_read,	
	.release	=	iom_buzzer_release,
};

// when buzzer device open ,call this function
int iom_buzzer_open(struct inode *minode, struct file *mfile) 
{	
	if(buzzer_port_usage != 0) return -EBUSY;

	buzzer_port_usage = 1;

	return 0;
}

// when buzzer device close ,call this function
int iom_buzzer_release(struct inode *minode, struct file *mfile) 
{
	buzzer_port_usage = 0;

	return 0;
}

// when write to buzzer device  ,call this function
ssize_t iom_buzzer_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) 
{
	unsigned char value;
    unsigned short int _s_value;
	const char *tmp = gdata;

	if (copy_from_user(&value, tmp, 1))
		return -EFAULT;

    _s_value = value & 0xF;

	outw(_s_value,(unsigned int)iom_fpga_buzzer_addr);	    
	
	return length;
}

// when read to buzzer device  ,call this function
ssize_t iom_buzzer_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) 
{
	unsigned char value;
    unsigned short int _s_value;
	char *tmp = gdata;

	_s_value = inw((unsigned int)iom_fpga_buzzer_addr);	    

    value = _s_value & 0xF;

	if (copy_to_user(tmp, &value, 1))
		return -EFAULT;

	return length;
}

int __init iom_buzzer_init(void)
{
	int result;

	result = register_chrdev(IOM_BUZZER_MAJOR, IOM_BUZZER_NAME, &iom_buzzer_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	iom_fpga_buzzer_addr = ioremap(IOM_BUZZER_ADDRESS, 0x1);

	printk("init module, %s major number : %d\n", IOM_BUZZER_NAME, IOM_BUZZER_MAJOR);

	return 0;
}

void __exit iom_buzzer_exit(void) 
{
	iounmap(iom_fpga_buzzer_addr);
	unregister_chrdev(IOM_BUZZER_MAJOR, IOM_BUZZER_NAME);
}

module_init(iom_buzzer_init);
module_exit(iom_buzzer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
