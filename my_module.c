#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int hello_init(void) {
    printk("Hello Kernel\n");
    return 0;
}

static void hello_exit(void) {
    printk("Goodbye this cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);