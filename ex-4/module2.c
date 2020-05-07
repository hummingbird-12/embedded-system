#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

struct my_data {
    int a, b, c;
};

struct _digits {
    char _1000;
    char _100;
    char _10;
    char _1;
};

extern long newcall(struct my_data *block);
extern long mynew_function(const int, struct _digits *);

int calldev_init(void) {
    struct my_data data;
    printk(KERN_INFO "Embedded SoftWare 2020\n");
    data.a = 7;
    data.b = 3;

    printk(KERN_INFO "sysc# = %ld\n", newcall(&data));

    printk(KERN_INFO "a + b = %d\n", data.a);
    printk(KERN_INFO "a - b = %d\n", data.b);
    printk(KERN_INFO "a %% b = %d\n", data.c);
    return 0;
}

int call_mynew_function() {
    const int number = 1577;
    struct _digits digits;

    printk(KERN_INFO "Embedded System Software - Practice 4\n");
    printk(KERN_INFO "sysc# = %ld\n\n", mynew_function(number, &digits));

    printk(KERN_INFO "Thousands =\t%d\n", digits._1000);
    printk(KERN_INFO "Hundreds  =\t%d\n", digits._100);
    printk(KERN_INFO "Tens      =\t%d\n", digits._10);
    printk(KERN_INFO "Units     =\t%d\n", digits._1);

    return 0;
}

void calldev_exit(void) {}
module_init(call_mynew_function);
module_exit(calldev_exit);
MODULE_LICENSE("Dual BSD/GPL");
