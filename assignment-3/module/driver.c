#include "core.h"
#include "driver_specs.h"

static int driver_port_usage = 0;

static int __init stopwatch_device_driver_init(void);
static void __exit stopwatch_device_driver_exit(void);
static int stopwatch_device_driver_open(struct inode *, struct file *);
static int stopwatch_device_driver_release(struct inode *, struct file *);
static int stopwatch_device_driver_write(struct file *, const char __user *,
                                         size_t, loff_t *);

static struct file_operations device_driver_fops = {
    .owner = THIS_MODULE,
    .open = stopwatch_device_driver_open,
    .release = stopwatch_device_driver_release,
    .write = stopwatch_device_driver_write,
};

/*
 * Called on `insmod`.
 * Registers the device driver, io-maps the FND device
 * and initializes timers.
 */
static int __init stopwatch_device_driver_init(void) {
    const int registration =
        register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_driver_fops);

    logger(INFO, "[stopwatch_device_driver] init\n");

    if (registration != 0) {
        logger(ERROR, "[stopwatch_device_driver] Failed registering device.\n");
        return registration;
    }

    logger(INFO, "[stopwatch_device_driver] device file: /dev/%s\n",
           DEVICE_NAME);
    logger(INFO, "[stopwatch_device_driver] device major number: %d\n",
           DEVICE_MAJOR_NUMBER);

    fpga_iomap_fnd();
    initizlize_timers();

    return SUCCESS;
}

/*
 * Called on `rmmod`.
 * Deregisters the device driver, deletes timer sync
 * and io-unmaps the FND device.
 */
static void __exit stopwatch_device_driver_exit(void) {
    logger(INFO, "[stopwatch_device_driver] exit\n");

    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    delete_timers_sync();
    fpga_iounmap_fnd();
}

/*
 * Called on `open()`.
 * Keeps track of driver's usage and registers interrupts.
 */
static int stopwatch_device_driver_open(struct inode *inode,
                                        struct file *file) {
    logger(INFO, "[stopwatch_device_driver] open\n");

    if (driver_port_usage != 0) {
        return -EBUSY;
    }
    driver_port_usage = 1;

    register_interrupts();

    return SUCCESS;
}

/*
 * Called on `close()`.
 * Keeps track of driver's usage and releases interrupts.
 */
static int stopwatch_device_driver_release(struct inode *inode,
                                           struct file *file) {
    logger(INFO, "[stopwatch_device_driver] release\n");

    driver_port_usage = 0;

    release_interrupts();

    return SUCCESS;
}

/*
 * Called on `write()`.
 * Initializes the stopwatch feature and puts app process into wait queue.
 */
static int stopwatch_device_driver_write(struct file *file,
                                         const char __user *buf, size_t count,
                                         loff_t *f_pos) {
    logger(INFO, "[stopwatch_device_driver] write\n");

    initialize_stopwatch();
    sleep_app();

    return 0;
}

module_init(stopwatch_device_driver_init);
module_exit(stopwatch_device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
