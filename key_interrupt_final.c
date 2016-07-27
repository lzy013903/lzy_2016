#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

//按键接口的物理地址
#define GPNCON 0x7F008830   
#define GPNDAT 0x7F008834           //数据寄存器

struct work_struct *work;           //工作队列结构
struct timer_list *lzy_timer;       //定时器结构
unsigned int *gpio_data;
unsigned int key_val = 0;           //按键数值
int flag = 0;                       //阻塞标记
wait_queue_head_t lzy_queue;        //等待队列

//实际的中断处理程序
void work_function(struct work_struct *work)
{
    //启动定时器，jiffies是当前时间，HZ是1秒
    mod_timer(lzy_timer, jiffies + (HZ/10));                                            
}

irqreturn_t key_isr(int irq, void *dev_id)
{
    schedule_work(work);    //提交需要做的工作
    return 0;
}

void timer_function(unsigned long data)
{
    key_val = readw(gpio_data)&0b1111;
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
    unsigned int* gpio_config;
    unsigned int data;
    
    gpio_config = ioremap(GPNCON,4);  //GPN的虚拟地址
    data = readl(gpio_config);
    data &= ~0b11111111;              //设置为中断模式
    data |=  0b10101010;
    writel(data,gpio_config);         //写寄存器
    gpio_data = ioremap(GPNDAT,4);    //数据寄存器内存地址
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

struct miscdevice key_misc_dev =    //这是一种乱序初始化方式，
{                                   //所以用的是逗号，是逗号
    .minor = 201,                   //次设备号，这个很关键
    .name = "key_lzy",              
    .fops = &key_file_operations,      
};

int key_lzy_init(void)
{
    //注册一个混杂设备
    misc_register(&key_misc_dev);
    
    //按键初始化
    key_hw_init();
   
    //下降沿触发中断
    request_irq(S3C_EINT(0),key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(S3C_EINT(1),key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(S3C_EINT(2),key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
    request_irq(S3C_EINT(3),key_isr,IRQF_TRIGGER_FALLING,"key_lzy",0);
   
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
    return 0;
}

void key_lzy_exit(void)
{
    misc_deregister(&key_misc_dev);
    free_irq(S3C_EINT(0),0);
    free_irq(S3C_EINT(1),0);
    free_irq(S3C_EINT(2),0);
    free_irq(S3C_EINT(3),0);
    kfree(work);
    work = NULL;
}

module_init(key_lzy_init);
module_exit(key_lzy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lzy");