#include "core.h"
#include "driver_specs.h"

static int driver_port_usage = 0;

static int __init device_driver_init(void);
static void __exit device_driver_exit(void);
static int timer_device_driver_open(struct inode*, struct file*);
static int timer_device_driver_release(struct inode*, struct file*);
static long timer_device_driver_ioctl(struct file*, unsigned int,
                                      unsigned long);

static struct file_operations device_driver_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = timer_device_driver_ioctl,
    .open = timer_device_driver_open,
    .release = timer_device_driver_release,
};

/*
 * Called on `insmod`.
 * Registers the device driver and io-maps the FPGA devices.
 */
static int __init device_driver_init(void) {
    const int registration =
        register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &device_driver_fops);

    logger(INFO, "[timer_device_driver] init\n");

    if (registration != 0) {
        logger(ERROR, "[timer_device_driver] Failed registering device.\n");
        return registration;
    }

    logger(INFO, "[timer_device_driver] device file: /dev/%s\n", DEVICE_NAME);
    logger(INFO, "[timer_device_driver] device major number: %d\n",
           DEVICE_MAJOR_NUMBER);

    fpga_iomap_devices();

    return SUCCESS;
}

/*
 * Called on `rmmod`.
 * Deregisters the device driver, deletes timer sync and io-unmaps the FPGA
 * devices.
 */
static void __exit device_driver_exit(void) {
    logger(INFO, "[timer_device_driver] exit\n");

    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    delete_timer();
    fpga_iounmap_devices();
}

/*
 * Called on `open()`.
 * Keeps track of driver's usage.
 */
static int timer_device_driver_open(struct inode* inode, struct file* file) {
    logger(INFO, "[timer_device_driver] open\n");

    if (driver_port_usage != 0) {
        return -EBUSY;
    }
    driver_port_usage = 1;

    return SUCCESS;
}

/*
 * Called on `close()`.
 * Keeps track of driver's usage.
 */
static int timer_device_driver_release(struct inode* inode, struct file* file) {
    logger(INFO, "[timer_device_driver] release\n");

    driver_port_usage = 0;

    return SUCCESS;
}

/*
 * Called on `ioctl()`.
 * Handles driver's features.
 */
static long timer_device_driver_ioctl(struct file* file, unsigned int ioctl_num,
                                      unsigned long ioctl_param) {
    char buffer[11], temp[4] = {'\0'};
    char* param;
    char timerInit[5] = {'\0'};
    long timerInterval, timerCount;

    switch (ioctl_num) {
        case IOCTL_SET_OPTION:  // IOCTL to receive parameters from user program
            logger(INFO, "[timer_device_driver] ioctl: IOCTL_SET_OPTION\n");

            // Get data from user program
            param = (char*) ioctl_param;
            if (strncpy_from_user(buffer, param, strlen_user(param)) < 0) {
                logger(ERROR,
                       "[timer_device_driver] Failed to read user pointer\n");
                return -1;
            }

            // Parse parameters
            strncpy(temp, buffer, 3);
            if (kstrtol(temp, 10, &timerInterval) != 0) {
                logger(ERROR,
                       "[timer_device_driver] Failed to parse parameter: %s\n",
                       temp);
                return -1;
            }
            strncpy(temp, buffer + 3, 3);
            if (kstrtol(temp, 10, &timerCount) != 0) {
                logger(ERROR,
                       "[timer_device_driver] Failed to parse parameter: %s\n",
                       temp);
                return -1;
            }
            strncpy(timerInit, buffer + 6, 4);

            initialize_timer(timerInit, timerInterval, timerCount);

            break;
        case IOCTL_COMMAND:  // IOCTL to start the timer feature
            logger(INFO, "[timer_device_driver] ioctl: IOCTL_COMMAND\n");

            start_timer();
            break;
        default:
            logger(
                WARN,
                "[timer_device_driver] ioctl: Unrecognized ioctl command %ud\n",
                ioctl_num);
            return -1;
            break;
    }

    return SUCCESS;
}

module_init(device_driver_init);
module_exit(device_driver_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Inho Kim <entropyih@sogang.ac.kr>");
