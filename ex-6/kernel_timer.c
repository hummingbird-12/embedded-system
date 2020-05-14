#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define KERNEL_TIMER_NAME "kernel_timer"
#define COUNT_DELAY_SECS 3

static int kernel_timer_usage = 0;

int kernel_timer_open(struct inode *, struct file *);
int kernel_timer_release(struct inode *, struct file *);
ssize_t kernel_timer_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations kernel_timer_fops = {
    .open = kernel_timer_open,
    .write = kernel_timer_write,
    .release = kernel_timer_release};

struct struct_mydata {
    struct timer_list timer;
    int count;
};

struct struct_mydata mydata;
int major;
static char *buffer;

int kernel_timer_release(struct inode *minode, struct file *mfile) {
    printk("kernel_timer_release\n");
    kernel_timer_usage = 0;
    return 0;
}

int kernel_timer_open(struct inode *minode, struct file *mfile) {
    printk("kernel_timer_open\n");
    if (kernel_timer_usage != 0) {
        return -EBUSY;
    }
    kernel_timer_usage = 1;
    return 0;
}

static void kernel_timer_blink(unsigned long timeout) {
    struct struct_mydata *p_data = (struct struct_mydata *) timeout;

    printk("kernel_timer_blink %d\n", p_data->count);

    (*buffer)++;
    p_data->count--;
    if (p_data->count < 0) {
        return;
    }

    mydata.timer.expires = get_jiffies_64() + (COUNT_DELAY_SECS * HZ);
    mydata.timer.data = (unsigned long) &mydata;
    mydata.timer.function = kernel_timer_blink;

    add_timer(&mydata.timer);
}

ssize_t kernel_timer_write(struct file *inode, const char *gdata, size_t length,
                           loff_t *off_what) {
    const char *tmp = gdata;
    char kernel_timer_buff = 0;

    printk("write\n");
    // 1 byte
    if (copy_from_user(&kernel_timer_buff, tmp, 1)) {
        return -EFAULT;
    }

    mydata.count = kernel_timer_buff;

    printk("data  : %d \n", mydata.count);

    del_timer_sync(&mydata.timer);

    mydata.timer.expires = jiffies + (COUNT_DELAY_SECS * HZ);
    mydata.timer.data = (unsigned long) &mydata;
    mydata.timer.function = kernel_timer_blink;

    add_timer(&mydata.timer);
    return 1;
}

int __init kernel_timer_init(void) {
    printk("kernel_timer_init\n");

    major = register_chrdev(0, KERNEL_TIMER_NAME, &kernel_timer_fops);
    if (major < 0) {
        printk("error %d\n", major);
        return major;
    }
    printk("dev_file : /dev/%s , major : %d\n", KERNEL_TIMER_NAME, major);

    init_timer(&(mydata.timer));

    // Allocate and initialize buffer
    buffer = (char *) kmalloc(sizeof(char), GFP_KERNEL);
    *buffer = 0;

    printk("init module\n");
    return 0;
}

void __exit kernel_timer_exit(void) {
    printk("kernel_timer_exit\n");
    kernel_timer_usage = 0;
    del_timer_sync(&mydata.timer);

    unregister_chrdev(major, KERNEL_TIMER_NAME);

    // Print and deallocate buffer
    printk("%d\n", *buffer);
    kfree(buffer);
}

module_init(kernel_timer_init);
module_exit(kernel_timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("author");
