#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/string.h>

#define DEVICE_NAME "warehouse"
#define CLASS_NAME "warehouse_class"
#define MAX_PRODUCTS 8
#define MAX_MESSAGE 256

static int major_number;
static struct class *warehouse_class;
static struct device *warehouse_device;
static struct cdev warehouse_cdev;

static DEFINE_MUTEX(warehouse_mutex);

struct product {
    char id[10];
    char name[30];
    int quantity;
    int price;
};

static struct product inventory[MAX_PRODUCTS] = {
    {"P101", "Laptop", 20, 55000},
    {"P102", "Keyboard", 35, 1200},
    {"P103", "Mouse", 50, 700},
    {"P104", "Monitor", 15, 12000},
    {"P105", "Printer", 10, 15000},
    {"P106", "Headphones", 25, 2000},
    {"P107", "Webcam", 18, 3000},
    {"P108", "USB_Cable", 60, 350}
};

static char message[MAX_MESSAGE] = "Warehouse Driver Ready\n";

static int find_product(const char *id)
{
    int i;

    for (i = 0; i < MAX_PRODUCTS; i++) {
        if (strcmp(inventory[i].id, id) == 0)
            return i;
    }

    return -1;
}

static int warehouse_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "warehouse: device opened\n");
    return 0;
}

static int warehouse_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "warehouse: device closed\n");
    return 0;
}

static ssize_t warehouse_read(struct file *file,
                              char __user *buffer,
                              size_t length,
                              loff_t *offset)
{
    size_t msg_len;
    size_t bytes;

    msg_len = strlen(message);

    if (*offset >= msg_len)
        return 0;

    bytes = msg_len - *offset;

    if (bytes > length)
        bytes = length;

    if (copy_to_user(buffer, message + *offset, bytes))
        return -EFAULT;

    *offset += bytes;

    return bytes;
}

static ssize_t warehouse_write(struct file *file,
                               const char __user *buffer,
                               size_t length,
                               loff_t *offset)
{
    char command[MAX_MESSAGE];
    char operation[30];
    char product_id[10];
    int quantity;
    int index;
    int ret;

    if (length == 0)
        return 0;

    if (length >= MAX_MESSAGE)
        length = MAX_MESSAGE - 1;

    if (copy_from_user(command, buffer, length))
        return -EFAULT;

    command[length] = '\0';

    command[strcspn(command, "\n")] = '\0';

    printk(KERN_INFO "warehouse: command -> %s\n", command);

    mutex_lock(&warehouse_mutex);

    /*
     * STOCK_IN P101 10
     */
    if (strncmp(command, "STOCK_IN", 8) == 0) {

        ret = sscanf(command,
                     "STOCK_IN %9s %d",
                     product_id,
                     &quantity);

        if (ret != 2) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid STOCK_IN command\n");
        }
        else if (quantity <= 0) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid quantity\n");
        }
        else {

            index = find_product(product_id);

            if (index == -1) {
                snprintf(message,
                         MAX_MESSAGE,
                         "ERROR: Product %s not found\n",
                         product_id);
            }
            else {

                inventory[index].quantity += quantity;

                snprintf(message,
                         MAX_MESSAGE,
                         "STOCK_IN SUCCESS: %s quantity = %d\n",
                         product_id,
                         inventory[index].quantity);

                printk(KERN_INFO
                       "warehouse: %s stock increased by %d\n",
                       product_id,
                       quantity);
            }
        }
    }

    /*
     * STOCK_OUT P101 5
     */
    else if (strncmp(command, "STOCK_OUT", 9) == 0) {

        ret = sscanf(command,
                     "STOCK_OUT %9s %d",
                     product_id,
                     &quantity);

        if (ret != 2) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid STOCK_OUT command\n");
        }
        else if (quantity <= 0) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid quantity\n");
        }
        else {

            index = find_product(product_id);

            if (index == -1) {
                snprintf(message,
                         MAX_MESSAGE,
                         "ERROR: Product %s not found\n",
                         product_id);
            }
            else if (quantity > inventory[index].quantity) {
                snprintf(message,
                         MAX_MESSAGE,
                         "ERROR: Insufficient stock. Available = %d\n",
                         inventory[index].quantity);
            }
            else {

                inventory[index].quantity -= quantity;

                snprintf(message,
                         MAX_MESSAGE,
                         "STOCK_OUT SUCCESS: %s quantity = %d\n",
                         product_id,
                         inventory[index].quantity);

                printk(KERN_INFO
                       "warehouse: %s stock decreased by %d\n",
                       product_id,
                       quantity);
            }
        }
    }

    /*
     * CHECK_STOCK P101
     */
    else if (strncmp(command, "CHECK_STOCK", 11) == 0) {

        ret = sscanf(command,
                     "CHECK_STOCK %9s",
                     product_id);

        if (ret != 1) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid CHECK_STOCK command\n");
        }
        else {

            index = find_product(product_id);

            if (index == -1) {
                snprintf(message,
                         MAX_MESSAGE,
                         "ERROR: Product %s not found\n",
                         product_id);
            }
            else {
                snprintf(message,
                         MAX_MESSAGE,
                         "STOCK: %s (%s) = %d units\n",
                         inventory[index].id,
                         inventory[index].name,
                         inventory[index].quantity);
            }
        }
    }

    /*
     * VIEW_INVENTORY
     */
    else if (strcmp(command, "VIEW_INVENTORY") == 0) {

        int i;
        int pos = 0;

        pos += snprintf(message + pos,
                        MAX_MESSAGE - pos,
                        "WAREHOUSE INVENTORY\n");

        for (i = 0; i < MAX_PRODUCTS; i++) {

            pos += snprintf(message + pos,
                            MAX_MESSAGE - pos,
                            "%s %s Qty=%d Price=%d\n",
                            inventory[i].id,
                            inventory[i].name,
                            inventory[i].quantity,
                            inventory[i].price);

            if (pos >= MAX_MESSAGE - 1)
                break;
        }
    }

    /*
     * SEARCH_PRODUCT P101
     */
    else if (strncmp(command, "SEARCH_PRODUCT", 14) == 0) {

        ret = sscanf(command,
                     "SEARCH_PRODUCT %9s",
                     product_id);

        if (ret != 1) {
            snprintf(message,
                     MAX_MESSAGE,
                     "ERROR: Invalid SEARCH_PRODUCT command\n");
        }
        else {

            index = find_product(product_id);

            if (index == -1) {
                snprintf(message,
                         MAX_MESSAGE,
                         "ERROR: Product %s not found\n",
                         product_id);
            }
            else {
                snprintf(message,
                         MAX_MESSAGE,
                         "PRODUCT: %s | %s | Qty=%d | Price=%d\n",
                         inventory[index].id,
                         inventory[index].name,
                         inventory[index].quantity,
                         inventory[index].price);
            }
        }
    }

    /*
     * Unknown command
     */
    else {

        snprintf(message,
                 MAX_MESSAGE,
                 "ERROR: Unknown command\n");
    }

    mutex_unlock(&warehouse_mutex);

    return length;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = warehouse_open,
    .read = warehouse_read,
    .write = warehouse_write,
    .release = warehouse_release
};

static int __init warehouse_init(void)
{
    dev_t dev;

    printk(KERN_INFO "warehouse: driver loading\n");

    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT
               "warehouse: failed to allocate device number\n");
        return -1;
    }

    major_number = MAJOR(dev);

    cdev_init(&warehouse_cdev, &fops);

    if (cdev_add(&warehouse_cdev, dev, 1) < 0) {
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    warehouse_class = class_create(CLASS_NAME);

    if (IS_ERR(warehouse_class)) {
        cdev_del(&warehouse_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(warehouse_class);
    }

    warehouse_device = device_create(
        warehouse_class,
        NULL,
        dev,
        NULL,
        DEVICE_NAME
    );

    if (IS_ERR(warehouse_device)) {
        class_destroy(warehouse_class);
        cdev_del(&warehouse_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(warehouse_device);
    }

    printk(KERN_INFO
           "warehouse: driver loaded successfully\n");

    printk(KERN_INFO
           "warehouse: /dev/%s created\n",
           DEVICE_NAME);

    return 0;
}

static void __exit warehouse_exit(void)
{
    dev_t dev = MKDEV(major_number, 0);

    device_destroy(warehouse_class, dev);
    class_destroy(warehouse_class);
    cdev_del(&warehouse_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "warehouse: driver unloaded\n");
}

module_init(warehouse_init);
module_exit(warehouse_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swagat");
MODULE_DESCRIPTION("Warehouse Management Character Device Driver");
MODULE_VERSION("2.0");
