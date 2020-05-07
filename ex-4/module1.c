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

long newcall(struct my_data* data) {
    int a, b;

    a = data->a;
    b = data->b;

    data->a = a + b;
    data->b = a - b;
    data->c = a % b;

    return 313;
}

int funcdev_init(void) { return 0; }
void funcdev_exit(void) {}

long mynew_function(const int num, struct _digits* digits) {
    digits->_1000 = (num / 1000) % 10;  // thousands
    digits->_100 = (num / 100) % 10;    // hundreds
    digits->_10 = (num / 10) % 10;      // tens
    digits->_1 = (num / 1) % 10;        // ones
    return 313;
}

EXPORT_SYMBOL(newcall);
EXPORT_SYMBOL(mynew_function);

module_init(funcdev_init);
module_exit(funcdev_exit);
MODULE_LICENSE("Dual BSD/GPL");
