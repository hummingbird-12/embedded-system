#include "core.h"
#include "timer_device.h"

static int __init device_driver_init(void);
static void __exit device_driver_exit(void);
static int timer_device_driver_open(struct inode*, struct file*);
static int timer_device_driver_release(struct inode*, struct file*);
static long timer_device_driver_ioctl(struct file*, unsigned int,
                                      unsigned long);

static struct file_operations device_driver_fops = {
    .unlocked_ioctl = timer_device_driver_ioctl,
    .open = timer_device_driver_open,
    .release = timer_device_driver_release,
};

static int __init device_driver_init(void) {
    const int registration =
        register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_driver_fops);

    printk("device_driver_init\n");

    if (registration != 0) {
        printk(KERN_ALERT "Error while registering %d\n", registration);
        return registration;
    }

    printk(KERN_INFO "dev file: /dev/%s\tmajor: %d\n", DEVICE_NAME,
           DEVICE_MAJOR_NUMBER);

    printk(KERN_INFO "init module\n");

    return SUCCESS;
}

static void __exit device_driver_exit(void) {
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);

    printk(KERN_INFO "device_driver_exit\n");
}

static int timer_device_driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "timer_device_driver_open\n");

    try_module_get(THIS_MODULE);

    return SUCCESS;
}

static int timer_device_driver_release(struct inode* inode, struct file* file) {
    printk(KERN_INFO "timer_device_driver_release\n");

    module_put(THIS_MODULE);

    return SUCCESS;
}

static long timer_device_driver_ioctl(struct file* file, unsigned int ioctl_num,
                                      unsigned long ioctl_param) {
    switch (ioctl_num) {
        case IOCTL_SET_OPTION:
            printk(KERN_INFO "[timer_device_driver_ioctl] IOCTL_SET_OPTION\n");
            break;
        case IOCTL_COMMAND:
            printk(KERN_INFO "[timer_device_driver_ioctl] IOCTL_COMMAND\n");
            break;
        default:
            printk(KERN_ALERT "Unrecognized ioctl command: %ud\n", ioctl_num);
            return -1;
            break;
    }

    return SUCCESS;
}

module_init(device_driver_init);
module_exit(device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
