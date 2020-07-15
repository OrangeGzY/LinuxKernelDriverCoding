#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include <linux/interrupt.h>
/*带参数 lnsmod时传递*/
static int irq;
module_param(irq,int,0644); //perm指定了在sysfs中相应文件的访问权限。访问权限与linux文件爱你访问权限相同的方式管理，如0644，或使用stat.h中的宏如S_IRUGO表示。
static char * interface;
module_param(interface,charp,0644);

/* 中断处理函数 返回类型irqreturn_t */
/* 
第一个参数irq是中断线号，dev_id是一个通用指针，用来区分共享同一中断处理程序的多个设备，比如两个一样的硬盘。
对于设备而言，设备结构是唯一的，通常把设备结构传递给dev_id。第三个参数regs是一个指向结构的指针，包含处理中断之前处理器的寄存器和状态。
\返回值的类型是reqreturn_t。中断处理程序可能返回两个特殊值：IRQ_NONE和IRQ_HANDLED。
当中断处理程序检测到一个中断，但该中断对应的设备不是注册处理函数时指定的源时，返回IRQ_NONE，正确调用，返回IRQ_HANDLED。
使用宏IRQ_RETVAL(x)，x为非0，宏返回IRQ_HANDLED；否则，返回IRQ_NONE。
利用这些值，内核可以知道设备发出的是否是一种虚假的中断。
中断处理程序一般是static，因为它不被别的文件中代码直接调用，static表明只在本代码文件可用。
*/
static irqreturn_t my_interrupter(int irq,void *dev_id, struct pt_regs *regs)
{
    static int count=0;
    if(count<10)
    {
        printk("Interrupt!");
        ++count;
    }
    return IRQ_NONE;
    /*
    * To mix old-style and new-style irq handler returns.
    * IRQ_NONE means we didn't handle it.
    * 中断程序接收到中断信号后发现这并不是注册时指定的中断原发出的中断信号.
    * 此时返回次值
    * IRQ_HANDLED means that we did have a valid interrupt and handled it.
    * 接收到了准确的中断信号,并且作了相应正确的处理
    * IRQ_RETVAL(x) selects on the two depending on x being non-zero (for handled)
    */
}





static int __init lkp_init(void)
{
    /* request_irq注册中断服务 */
    if(request_irq(IRQ_EINT(irq),my_interrupter, IRQF_SHARED,interface,&irq))    //IRQF_SHARED则表示多个设备共享中断
    {
        printk("Cannot register IRQ: %d\n",irq);
        return -EIO;
    }
    printk("%s Requests IRQ: %d success!\n",interface,irq);
    return 0;
}

static int __exit lkp_exit(void)
{
    printk("Uninstalling...\n");
    free_irq(irq,&irq);
    printk("free IRQ:%d\n",irq);
    return 0; 
}


module_init(lkp_init);
module_exit(lkp_exit);
MODULE_LICENSE("GPL"); 