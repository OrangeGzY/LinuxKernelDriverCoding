#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>//编写内核模块所需要的一些头文件，比如模块注册和注销的宏等
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <linux/path.h>
#include <linux/time.h>
#include <linux/stat.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <asm/cpufeature.h>


unsigned long **sys_call_table = (unsigned int **)0xffffffff81a00200; //系统调用表的地址
unsigned long *orig_mkdir = NULL;  //指向原本的系统调用地址


asmlinkage void hacked_mkdir(const char __user *pathname, int mode)
{
    printk("mkdir pathname: %s\n",pathname);
   //printk(KERN_ALERT"mkdir do nothing!\n");
    return;
}

/* 改这一页可写 */
int make_rw(unsigned long address)
{
    unsigned long level;
    /* 找到相关的页表 */
    pte_t *pte = lookup_address(address, &level);
    /* 设置页表读写属性 */
    if(pte->pte & ~_PAGE_RW)
    {
        pte->pte |= _PAGE_RW;
    }
    return 0;
}

/* 恢复权限 */
int make_ro(unsigned long address)
{
    unsigned long level;
    pte_t *pte = lookup_address(address, &level);
    /* 设置只读 */
    pte->pte & ~_PAGE_RW;
    return 0;
}



/* 在模块载入的时候，先保存原有的系统调用的地址，然后再修改sys_call_table所在页表的属性，为可写的，然后再赋值为新的我们自己的系统调用地址 */
static int __init syscall_init(void)
{
    /* 系统调用表地址 */
    printk(KERN_ALERT"sys_call_table: 0x%p\n",sys_call_table);
    /* 获取原来的系统调用地址 */
    orig_mkdir = (unsigned int*)(sys_call_table[__NR_mkdir]);
    printk(KERN_ALERT"orig_mkdir: 0x%p\n",orig_mkdir);
    /* 修改页属性 */
    make_rw((unsigned long)sys_call_table);
    /* 设置新的系统调用地址 */
    sys_call_table[__NR_mkdir] = (unsigned long*)hacked_mkdir;

    make_ro((unsigned long)sys_call_table);
    return 0;
}
/*  卸载模块的时候注意需要将原有的系统调用进行还原 */
static void __exit syscall_exit(void)
{
        printk(KERN_ALERT "Module syscall unloaded.\n");

        make_rw((unsigned long)sys_call_table);
        sys_call_table[__NR_mkdir] = (unsigned long *)orig_mkdir;        /*set mkdir syscall to the origal one*/
        make_ro((unsigned long)sys_call_table);
}

module_init(syscall_init);
module_exit(syscall_exit);
//MOUDLE_LICENSE("GPL");
MODULE_LICENSE("GPL");
