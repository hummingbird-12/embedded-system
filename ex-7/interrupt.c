#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <mach/gpio.h>

static int inter_major = 0, inter_minor = 0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;
static int inter_open(struct inode*, struct file*);
static int inter_release(struct inode*, struct file*);
static int inter_write(struct file* filp, const char* buf, size_t count,
                       loff_t* f_pos);

irqreturn_t inter_handler1(int irq, void* dev_id);
irqreturn_t inter_handler2(int irq, void* dev_id);
irqreturn_t inter_handler3(int irq, void* dev_id);
irqreturn_t inter_handler4(int irq, void* dev_id);

int interruptCount = 0;

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);

static struct file_operations inter_fops = {
    .open = inter_open,
    .write = inter_write,
    .release = inter_release,
};

// HOME button interrupt handler
irqreturn_t inter_handler1(int irq, void* dev_id) {
    printk(KERN_ALERT "interrupt1!!! = %x\n",
           gpio_get_value(IMX_GPIO_NR(1, 11)));

    // if (++interruptCount >= 3) {
    //     interruptCount = 0;
    //     __wake_up(&wq_write, 1, 1, NULL);
    //     // wake_up_interruptible(&wq_write);
    //     printk("wake up\n");
    // }

    return IRQ_HANDLED;
}

// BACK button interrupt handler
irqreturn_t inter_handler2(int irq, void* dev_id) {
    printk(KERN_ALERT "interrupt2!!! = %x\n",
           gpio_get_value(IMX_GPIO_NR(1, 12)));
    return IRQ_HANDLED;
}

// VOLUP button interrupt handler
irqreturn_t inter_handler3(int irq, void* dev_id) {
    printk(KERN_ALERT "interrupt3!!! = %x\n",
           gpio_get_value(IMX_GPIO_NR(2, 15)));

    if (++interruptCount >= 5) {
        interruptCount = 0;
        __wake_up(&wq_write, 1, 1, NULL);
        printk("wake up\n");
    }

    return IRQ_HANDLED;
}

// VOLDOWN button interrupt handler
irqreturn_t inter_handler4(int irq, void* dev_id) {
    printk(KERN_ALERT "interrupt4!!! = %x\n",
           gpio_get_value(IMX_GPIO_NR(5, 14)));
    return IRQ_HANDLED;
}

/*
 * Handler for open()
 * Register interrupts
 */
static int inter_open(struct inode* minode, struct file* mfile) {
    int ret;
    int irq;

    printk(KERN_ALERT "Open Module\n");

    // Register HOME button interrupt
    gpio_direction_input(IMX_GPIO_NR(1, 11));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler1, IRQF_TRIGGER_RISING, "home", 0);

    // Register BACK button interrupt
    gpio_direction_input(IMX_GPIO_NR(1, 12));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler2, IRQF_TRIGGER_RISING, "back", 0);

    // Register VOLUP button interrupt
    gpio_direction_input(IMX_GPIO_NR(2, 15));
    irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler3, IRQF_TRIGGER_RISING, "volup", 0);

    // Register VOLDOWN button interrupt
    gpio_direction_input(IMX_GPIO_NR(5, 14));
    irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, inter_handler4, IRQF_TRIGGER_RISING, "voldown", 0);

    return 0;
}

/*
 * Handler for close()
 * De-register interrupts
 */
static int inter_release(struct inode* minode, struct file* mfile) {
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

    printk(KERN_ALERT "Release Module\n");
    return 0;
}

/*
 * Handler for write()
 * Sleep until 3 interrupts
 */
static int inter_write(struct file* filp, const char* buf, size_t count,
                       loff_t* f_pos) {
    if (interruptCount == 0) {
        printk("sleep on\n");
        interruptible_sleep_on(&wq_write);
    }
    printk("write\n");
    return 0;
}

// Device registration and initialization
static int inter_register_cdev(void) {
    int error;
    if (inter_major) {
        inter_dev = MKDEV(inter_major, inter_minor);
        error = register_chrdev_region(inter_dev, 1, "inter");
    } else {
        error = alloc_chrdev_region(&inter_dev, inter_minor, 1, "inter");
        inter_major = MAJOR(inter_dev);
    }
    if (error < 0) {
        printk(KERN_WARNING "inter: can't get major %d\n", inter_major);
        return result;
    }
    printk(KERN_ALERT "major number = %d\n", inter_major);
    cdev_init(&inter_cdev, &inter_fops);
    inter_cdev.owner = THIS_MODULE;
    inter_cdev.ops = &inter_fops;
    error = cdev_add(&inter_cdev, inter_dev, 1);
    if (error) {
        printk(KERN_NOTICE "inter Register Error %d\n", error);
    }
    return 0;
}

// Called on `insmod`
static int __init inter_init(void) {
    int result;
    if ((result = inter_register_cdev()) < 0) return result;
    printk(KERN_ALERT "Init Module Success \n");
    printk(KERN_ALERT "Device : /dev/inter, Major Num : 246 \n");
    return 0;
}

// Called on `rmmod`
static void __exit inter_exit(void) {
    cdev_del(&inter_cdev);
    unregister_chrdev_region(inter_dev, 1);
    printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
