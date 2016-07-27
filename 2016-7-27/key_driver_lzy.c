#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/platform_device.h>

struct work_struct *work;           //工作队列结构
struct timer_list *lzy_timer;       //定时器结构
unsigned int *gpio_data;
unsigned int key_val = 0;           //按键数值
int flag = 0;                       //阻塞标记
wait_queue_head_t lzy_queue;        //等待队列
struct resource *res_irq;					//保存资源数据
struct resource *res_mem;					//保存资源数据
unsigned int *key_base;						//保存资源数据

//实际的中断处理程序
void work_function(struct work_struct *work)
{
    mod_timer(lzy_timer, jiffies + (HZ/10));                                            
}

irqreturn_t key_isr(int irq, void *dev_id)
{
    schedule_work(work);    //提交需要做的工作
    return 0;
}

void timer_function(unsigned long data)
{
    key_val = readw(key_base+1)&0b1111;
    switch(key_val)
    {
        case 0b1110 :   
            printk(KERN_WARNING"key1 has been down!!\n");
            break;
        case 0b1101 :   
            printk(KERN_WARNING"key2 has been down!!\n");
            break;
        case 0b1011 :   
            printk(KERN_WARNING"key3 has been down!!\n");
            break;           
        case 0b0111 :   
            printk(KERN_WARNING"key4 has been down!!\n");
            break;
        default:    break;
    }
    flag = 1;
    wake_up(&lzy_queue);
}

void key_hw_init(void)                //硬件初始化步骤
{
    unsigned int data;
    
	data = readl(key_base);
	data &= ~0b11111111	;			//中断模式
	data |=  0b10101010	;

    writel(data,key_base);			//写寄存器
}

int key_open(struct inode *node, struct file *filp)
{
    return 0; 
}

int key_read(struct file *filp, char __user *buffer,
             size_t size, loff_t *pos)
{
    wait_event(lzy_queue,flag);
    flag=0;
    copy_to_user(buffer,&key_val,4);
    return 4; 
}

const struct file_operations key_file_operations =
{
    .open = key_open,
    .read = key_read,
};

struct miscdevice key_misc_dev =    
{                                   
    .minor = 201,                   
    .name = "key_lzy",              
    .fops = &key_file_operations,      
};

static int __devinit key_probe(struct platform_device *pdev)
{
	int size=0;
	int ret;
    ret = misc_register(&key_misc_dev);
    
	//获取硬件信息
    res_mem = platform_get_resource(pdev,IORESOURCE_MEM,0);
	size = (res_mem->end) - (res_mem->start) + 1;
	key_base = ioremap(res_mem->start,size);

    //按键初始化
    key_hw_init();
    
   	res_irq = platform_get_resource(pdev,IORESOURCE_IRQ,0);
    //下降沿触发中断
    request_irq(res_irq->start  ,key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(res_irq->start+1,key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(res_irq->start+2,key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(res_irq->start+3,key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
   
    //创建一个工作队列
    work = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work, work_function);

    //创建一个定时器
    lzy_timer = kmalloc(sizeof(struct timer_list),GFP_KERNEL);
    init_timer(lzy_timer);
    lzy_timer->function = timer_function;   //这个必须手动设置
    add_timer(lzy_timer);                   //向内核注册定时器

    //初始化等待队列
    init_waitqueue_head(&lzy_queue);

    printk(KERN_WARNING"key interrupt module init is OK!!!\n");
    return ret;
}

static int __devinit key_remove(struct platform_device *pdev)
{
	misc_deregister(&key_misc_dev);
	return 0;
}

struct platform_driver key_driver = {
	.probe   =  key_probe,
	.remove  =   __devexit_p(key_remove),
	.driver  = {
			.name = "lzy-key",		//名字必须必配
	}
};

int key_lzy_init(void)
{
   	platform_driver_register(&key_driver);
	return 0;
}

void key_lzy_exit(void)
{
    platform_driver_unregister(&key_driver);
}

module_init(key_lzy_init);
module_exit(key_lzy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lzy");