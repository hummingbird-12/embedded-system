#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DEVICE_DRIVER_NAME "driver_test"

int major_number;
int number;

int device_driver_init(void);
void device_driver_exit(void);

int test_device_driver_open(struct inode*, struct file*);
int test_device_driver_release(struct inode*, struct file*);
ssize_t test_device_driver_write(struct file*, const char*, size_t, loff_t*);
ssize_t test_device_driver_read(struct file*, char*, size_t, loff_t*);

static struct file_operations device_driver_fops = {
    .open = test_device_driver_open,
    .read = test_device_driver_read,
    .write = test_device_driver_write,
    .release = test_device_driver_release,
};

int device_driver_init(void) {
    printk("device_driver_init\n");

    major_number = register_chrdev(0, DEVICE_DRIVER_NAME, &device_driver_fops);
    if (major_number < 0) {
        printk("error %d\n", major_number);
        return major_number;
    }

    printk("dev file: /dev/%s\tmajor: %d\n", DEVICE_DRIVER_NAME, major_number);

    printk("init module\n");

    return 0;
}

void device_driver_exit(void) {
    printk("device_driver_exit\n");

    unregister_chrdev(major_number, DEVICE_DRIVER_NAME);
}

int test_device_driver_open(struct inode* minode, struct file* mfile) {
    printk("test_device_driver_open\n");
    return 0;
}

int test_device_driver_release(struct inode* minode, struct file* mfile) {
    printk("test_device_driver_release\n");
    return 0;
}

ssize_t test_device_driver_write(struct file* mfile, const char* gdata,
                                 size_t length, loff_t* off_what) {
    char kernel_buff[4];

    printk("Write\n");
    if (copy_from_user(kernel_buff, gdata, length)) {
        return -EFAULT;
    }

    number = simple_strtol(kernel_buff, NULL, 10);
    printk("Number: %d\n", number);

    return 1;
}

ssize_t test_device_driver_read(struct file* mfile, char* gdata, size_t length, loff_t* off_what) {
    char buff = 0;

    printk("Read\n");

    while (number != 0) {
        buff += number % 10;
        number /= 10;
    }
    printk("Rst: %d\n", buff);

    if (copy_to_user(gdata, &buff, length)) {
        return -EFAULT;
    }

    return 1;
}

module_init(device_driver_init);
module_exit(device_driver_exit);
MODULE_LICENSE("Dual BSD/GPL");
