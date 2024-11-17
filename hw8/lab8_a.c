#include <linux/module.h>    
#include <linux/kernel.h>    
#include <linux/init.h>      
#include <linux/fs.h>       
#include <linux/miscdevice.h>
#include <linux/sched.h>     
#include <linux/cred.h>
     
#define DEVICE_NAME "lab8"   

static uid_t saved_uid;      // store the UID of the process that opens the device

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shounak Ghosh");
MODULE_DESCRIPTION("Assignment 8 - Character Device Driver");

// Open function for the misc device
static int misc_open(struct inode *inode, struct file *file)
{
    // Save the UID of the process that opened the device
    saved_uid = current_uid().val;

    // Log the UID to the kernel log
    printk(KERN_INFO "lab8: Device opened by UID %d\n", saved_uid);
    return 0; 
}

// Read function for the misc device
static ssize_t misc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    // count is our buffer size from the user space
    char uid_str[16];    // Buffer to store the UID as a string (16 is the max length)
    int len;             // Length of the UID string

    // Convert the saved UID to a string and store it in the buffer
    len = snprintf(uid_str, sizeof(uid_str), "%u", saved_uid);

    // If the file pointer is beyond the length of the string, indicate EOF
    if (*ppos >= len)
        return 0;

    // Adjust the count to avoid reading beyond the string's length
    if (count > len - *ppos)
        count = len - *ppos;

    // Copy data from kernel space to user space
    if (copy_to_user(buf, uid_str + *ppos, count))
        return -EFAULT; // Return error if copy fails

    // Update the file pointer
    *ppos += count;
    return count; // Return the number of bytes read
}

// File operations structure
static const struct file_operations lab8_fops = {
    .owner = THIS_MODULE,  // Module owner
    .open = misc_open,     // Open function
    .read = misc_read,     // Read function
};

// Misc device structure
static struct miscdevice lab8_device = {
    .minor = MISC_DYNAMIC_MINOR, // Dynamically allocate a minor number
    .name = DEVICE_NAME,         // Name of the device
    .fops = &lab8_fops,          // File operations associated with the device
    .mode = S_IRUGO,             // Device permissions (read-only for everyone)
};

// Module initialization function
static int __init misc_init(void)
{
    int ret;

    ret = misc_register(&lab8_device);
    if (ret) {
        printk(KERN_ERR "lab8: Failed to register misc device\n");
        return ret; // Return error if registration fails
    }

    printk(KERN_INFO "lab8: Module loaded\n");
    return 0; 
}

// Module cleanup function
static void __exit misc_exit(void)
{
    misc_deregister(&lab8_device);

    printk(KERN_INFO "lab8: Module unloaded\n");
}

// Specify the initialization and cleanup functions
module_init(misc_init);
module_exit(misc_exit);
