#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "gpio_light"
#define DRIVER_CLASS "MyModuleClass"

MODULE_LICENSE("GPL");

static dev_t my_device_number;
static struct class *my_class;
static struct cdev my_device;

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char tmp[3] = " \n";

    to_copy = min(count, sizeof(tmp));

    printk("Value of button: %d\n", gpio_get_value(17));
    tmp[0] = gpio_get_value(17) + '0';

    not_copied = copy_to_user(user_buffer, &tmp, to_copy);

    delta = to_copy - not_copied;

    return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char value;

    to_copy = min(count, sizeof(value));

    not_copied = copy_from_user(&value, user_buffer, to_copy);

    switch (value) {
        case '0':
            gpio_set_value(4, 0);
            break;
        case '1':
            gpio_set_value(4, 1);
            break;
        default:
            printk("Invalid Input\n");
            break;
    }

    delta = to_copy - not_copied;

    return delta;
}

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
    .read = driver_read,
    .write = driver_write,
};

static int __init hello_init(void) {
    printk("Hello Kernel\n");

    if (alloc_chrdev_region(&my_device_number, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Number could not be alloced\n");
        return -1;
    }
    printk("Driver Registered Number Major: %d, Minor: %d", my_device_number >> 20, my_device_number & 0xfffff);

    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device Class cannot be created\n");
        goto ClassError;
    }

    if (device_create(my_class, NULL, my_device_number, NULL, DRIVER_NAME) == NULL) {
        printk("Cannot Create device file\n");
        goto FileError;
    }

    // device file
    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_number, 1) == -1) {
        printk("Registering device to kernel failed\n");
        goto AddError;
    }

    if (gpio_request(4, "rpi-gpio")) {
        printk("Cannot alloc GPIO 4\n");
        goto AddError;
    }

    if (gpio_direction_output(4, 0)) {
        printk("Cannot set GPIO 4 to output\n");
        goto GPIO4Error;
    }

    if (gpio_request(17, "rpi-gpio-17")) {
        printk("Cannot alloc GPIO 17\n");
        goto GPIO4Error;
    }

    if (gpio_direction_input(17)) {
        printk("Cannot set GPIO 17 to output\n");
        goto GPIO17Error;
    }

    return 0;

GPIO17Error:
    gpio_free(17);
GPIO4Error:
    gpio_free(4);
AddError:
    device_destroy(my_class, my_device_number);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_number, DRIVER_NAME);

    return -1;
}

static void __exit hello_exit(void) {
    gpio_set_value(4, 1);
    gpio_free(17);
    gpio_free(4);
    device_destroy(my_class, my_device_number);
    class_destroy(my_class);
    printk("Goodbye this cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);