#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Shounak Ghosh");
MODULE_DESCRIPTION("A simple Linux kernel module with timekeeping features");

static unsigned long start_jiffies;
static ktime_t start_time;

static int __init simple_module_init(void)
{
    start_jiffies = jiffies;
    start_time = ktime_get_boottime();

    printk(KERN_INFO "Hello, world\n");
    // Use correct format specifier for '1000 / HZ' which is an integer
    printk(KERN_INFO "Current tick time in ms: %d\n", 1000 / HZ);
    
    return 0;
}

static void __exit simple_module_exit(void)
{
    unsigned long end_jiffies = jiffies;
    ktime_t end_time = ktime_get_boottime();

    // Calculate elapsed time in jiffies
    unsigned long elapsed_jiffies = end_jiffies - start_jiffies;
    printk(KERN_INFO "Goodbye, cruel world\n");
    // Corrected format specifier for elapsed_jiffies which is an unsigned long
    printk(KERN_INFO "Time elapsed using jiffies: %lu ms\n", (elapsed_jiffies * 1000) / HZ);

    // Calculate elapsed time using ktime
    s64 elapsed_time_ns = ktime_to_ns(ktime_sub(end_time, start_time));
    // Corrected format specifier for s64 which is long long int
    printk(KERN_INFO "Time elapsed using ktime: %lld ms\n", elapsed_time_ns / 1000000);
}

module_init(simple_module_init);
module_exit(simple_module_exit);
