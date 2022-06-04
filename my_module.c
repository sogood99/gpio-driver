#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#define MAJORDEVICE 90

MODULE_LICENSE("GPL");

static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_nr - open was called\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_nr - close was called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
};

static int hello_init(void) {
    int retval;

    printk("Hello Kernel\n");

    retval = register_chrdev(MAJORDEVICE, "my_dev_nr", &fops);
    if (retval == 0) {
        printk("dev_nr - registered device number Major: %d, Minor: %d\n", MAJORDEVICE, 0);
    } else if (retval > 0) {
        printk("dev_nr - registered device number Major: %d, Minor: %d\n", retval >> 20, retval & 0xfffff);
    } else {
        printk("Could not register device number\n");
    }

    return 0;
}

static void hello_exit(void) {
    unregister_chrdev(MAJORDEVICE, "my_dev_nr");
    printk("Goodbye this cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);