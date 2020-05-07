#include <linux/kernel.h>
#include <linux/uaccess.h>

struct int_pair {
    int n1;
    int n2;
};

asmlinkage int sys_newcall3(struct int_pair *pair) {
    struct int_pair my_pair;
    copy_from_user(&my_pair, pair, sizeof(my_pair));

    const int val1 = my_pair.n1;
    const int val2 = my_pair.n2;
    const int sum = val1 + val2;

    printk("[sys_newcall3] Number1:\t %d\n", val1);
    printk("[sys_newcall3] Number2:\t %d\n", val2);
    printk("[sys_newcall3] Sum:\t %d\n", sum);

    return 23;
}

