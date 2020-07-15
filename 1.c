#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

static int __init lkp_init(void)
{
    printk("hello word! from the kernel space...\n");   // insmod 的时候调用
    return 0;
}

static int __exit lkp_exit()
{
    printk("Goodbye,world! leaving kernel space...\n"); // rmmod的时候调用
}


module_init(lkp_init);
module_exit(lkp_exit);
MODULE_LICENSE("GPL"); 