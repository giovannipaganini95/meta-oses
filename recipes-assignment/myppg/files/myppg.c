#include <asm/errno.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include "data.h"

#define DEBUG 0
#if DEBUG
#define DEBUG_KPRINT(...) printk(__VA_ARGS__)
#else
#define DEBUG_KPRINT(...)
#endif

#define DEVICE_NAME "myppg"
#define DATA_SIZE 4
typedef unsigned int ppg_data_t;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Giovanni Paganini");
MODULE_DESCRIPTION("Virtual PPG Sensor Linux Kernel Module.");
MODULE_VERSION("0.01");

static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *filp, char __user *buff, size_t len, loff_t *f_pos);
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *f_pos);

static int device_open_count = 0;

static dev_t ppg_dev;
struct cdev ppg_cdev;
struct class *ppg_class = NULL;

static struct file_operations ppg_fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release};

static ssize_t device_read(struct file *flip, char *buff, size_t len, loff_t *f_pos)
{
    ppg_data_t tmp_data = 0;
    int bytes_read = 0;
    int k_off = *f_pos;
    int u_off = 0;
    while (len >= DATA_SIZE)
    {
        tmp_data = PPG_ARRAY[k_off];
        if (copy_to_user(&buff[u_off], &tmp_data, DATA_SIZE) != 0)
        {
            DEBUG_KPRINT(KERN_ALERT "%s: read error, copy_to_user\n", DEVICE_NAME);
            return -EIO;
        }
        DEBUG_KPRINT(KERN_INFO "%s: read done, value=%d\n", DEVICE_NAME, tmp_data);
        len -= DATA_SIZE;
        k_off = (k_off + 1) % PPG_ARRAY_SIZE;
        u_off += DATA_SIZE;
        bytes_read += DATA_SIZE;
    }

    *f_pos = k_off;
    
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *f_pos)
{
    DEBUG_KPRINT(KERN_ALERT "%s: write error, operation not permitted\n", DEVICE_NAME);
    return -EPERM;
}

static int device_open(struct inode *inode, struct file *file)
{
    if (device_open_count)
    {
        DEBUG_KPRINT(KERN_ALERT "%s: open error, device busy\n", DEVICE_NAME);
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    DEBUG_KPRINT(KERN_INFO "%s: open done\n", DEVICE_NAME);
    
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    device_open_count--;
    module_put(THIS_MODULE);
    DEBUG_KPRINT(KERN_INFO "[%s] release done\n", DEVICE_NAME);
    
    return 0;
}
static int __init lkm_ppg_init(void)
{
    if (alloc_chrdev_region(&ppg_dev, 0, 1, "myppg_dev") < 0)
    {
        printk(KERN_ALERT "%s: init error, alloc_chrdev_region\n", DEVICE_NAME);
        return -EPERM;
    }
    printk(KERN_INFO "%s: init done, major no: %d, minor no: %d\n", DEVICE_NAME, MAJOR(ppg_dev), MINOR(ppg_dev));

    ppg_class = class_create(THIS_MODULE, "myppg_class");
    device_create(ppg_class, NULL, ppg_dev, NULL, "myppg_dev");

    cdev_init(&ppg_cdev, &ppg_fops);
    ppg_cdev.owner = THIS_MODULE;
    cdev_add(&ppg_cdev, ppg_dev, 1);

    return 0;
}

static void __exit lkm_ppg_exit(void)
{
    printk(KERN_INFO "%s: clean-up done\n", DEVICE_NAME);
    device_destroy(ppg_class, ppg_dev);
    cdev_del(&ppg_cdev);
    class_destroy(ppg_class);
    unregister_chrdev_region(ppg_dev, 1);
}

module_init(lkm_ppg_init);
module_exit(lkm_ppg_exit);