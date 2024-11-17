#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/cred.h>

#define DEVICE_NAME "lab8"

static uid_t saved_uid;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shounak Ghosh");
MODULE_DESCRIPTION("Assignment 8 - Character Device Driver");

static int misc_open(struct inode *inode, struct file *file)
{
    saved_uid = current_uid().val;
    printk(KERN_INFO "lab8: Device opened by UID %d\n", saved_uid);
    return 0;
}

static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    char uid_str[16];
    int len;

    len = snprintf(uid_str, sizeof(uid_str), "%u", saved_uid);

    if (*ppos >= len)
        return 0;

    if (count > len - *ppos)
        count = len - *ppos;

    if (copy_to_user(buf, uid_str + *ppos, count))
        return -EFAULT;

    *ppos += count;
    return count;
}

static const struct file_operations lab8_fops = {
    .owner = THIS_MODULE,
    .open = misc_open,
    .read = misc_read,
};

static struct miscdevice lab8_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &lab8_fops,
    .mode = S_IRUGO,
};

static int __init misc_init(void)
{
    int ret;

    ret = misc_register(&lab8_device);
    if (ret) {
        printk(KERN_ERR "lab8: Failed to register misc device\n");
        return ret;
    }

    printk(KERN_INFO "lab8: Module loaded\n");
    return 0;
}

static void __exit misc_exit(void)
{
    misc_deregister(&lab8_device);
    printk(KERN_INFO "lab8: Module unloaded\n");
}

module_init(misc_init);
module_exit(misc_exit);

