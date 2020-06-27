#include "core.h"
#include "driver_specs.h"

static int driver_port_usage = 0;

static int __init hangman_device_driver_init(void);
static void __exit hangman_device_driver_exit(void);
static int hangman_device_driver_open(struct inode *, struct file *);
static int hangman_device_driver_release(struct inode *, struct file *);
static int hangman_device_driver_write(struct file *, const char __user *,
                                       size_t, loff_t *);

static struct file_operations device_driver_fops = {
    .owner = THIS_MODULE,
    .open = hangman_device_driver_open,
    .release = hangman_device_driver_release,
    .write = hangman_device_driver_write,
};

/*
 * Called on `insmod`.
 * Registers the device driver, io-maps the FND device
 * and initializes timers.
 */
static int __init hangman_device_driver_init(void) {
    const int registration =
        register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_driver_fops);

    logger(INFO, "[hangman_device_driver] init\n");

    if (registration != 0) {
        logger(ERROR, "[hangman_device_driver] Failed registering device.\n");
        return registration;
    }

    logger(INFO, "[hangman_device_driver] device file: /dev/%s\n", DEVICE_NAME);
    logger(INFO, "[hangman_device_driver] device major number: %d\n",
           DEVICE_MAJOR_NUMBER);

    fpga_iomap_devices();
    // initizlize_timers();

    return SUCCESS;
}

/*
 * Called on `rmmod`.
 * Deregisters the device driver, deletes timer sync
 * and io-unmaps the FND device.
 */
static void __exit hangman_device_driver_exit(void) {
    logger(INFO, "[hangman_device_driver] exit\n");

    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    // delete_timers_sync();
    fpga_iounmap_devices();
}

/*
 * Called on `open()`.
 * Keeps track of driver's usage and registers interrupts.
 */
static int hangman_device_driver_open(struct inode *inode, struct file *file) {
    logger(INFO, "[hangman_device_driver] open\n");

    if (driver_port_usage != 0) {
        return -EBUSY;
    }
    driver_port_usage = 1;

    // register_interrupts();

    return SUCCESS;
}

/*
 * Called on `close()`.
 * Keeps track of driver's usage and releases interrupts.
 */
static int hangman_device_driver_release(struct inode *inode,
                                         struct file *file) {
    logger(INFO, "[hangman_device_driver] release\n");

    driver_port_usage = 0;

    // release_interrupts();

    return SUCCESS;
}

/*
 * Called on `write()`.
 * Initializes the hangman feature and puts app process into wait queue.
 */
static int hangman_device_driver_write(struct file *file,
                                       const char __user *buf, size_t count,
                                       loff_t *f_pos) {
    logger(INFO, "[hangman_device_driver] write\n");

    // initialize_stopwatch();
    // sleep_app();

    return 0;
}

module_init(hangman_device_driver_init);
module_exit(hangman_device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
