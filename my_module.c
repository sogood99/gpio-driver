#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "MyModuleClass"

MODULE_LICENSE("GPL");

static char buffer[255];
static int buffer_pointer;

static dev_t my_device_number;
static struct class *my_class;
static struct cdev my_device;

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    to_copy = min(count, buffer_pointer);

    not_copied = copy_to_user(user_buffer, buffer, to_copy);

    delta = to_copy - not_copied;

    return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;

    to_copy = min(count, sizeof(buffer));

    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    buffer_pointer = to_copy;

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

static int hello_init(void) {
    int retval;

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

    return 0;
AddError:
    device_destroy(my_class, my_device_number);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_number, DRIVER_NAME);

    return -1;
}

static void hello_exit(void) {
    cdev_del(&my_device);
    device_destroy(my_class, my_device_number);
    class_destroy(my_class);
    unregister_chrdev(my_device_number, DRIVER_NAME);
    printk("Goodbye this cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);