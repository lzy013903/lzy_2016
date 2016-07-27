#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/platform_device.h>

struct work_struct *work1;
struct timer_list buttons_timer;
unsigned int key_val=0b111111;
wait_queue_head_t key_que;					//等待队列
int flag=0;	
struct resource *res_irq;					//保存资源数据
struct resource *res_mem;					//保存资源数据
unsigned int *key_base;						//保存资源数据

MODULE_LICENSE("GPL");						//使用协议

//---------------------------中断下半部函数----------------------------
void work1_func(struct work_struct *work)			
{
	 mod_timer(&buttons_timer, jiffies + (HZ /10)); //启动定时器	
}

//---------------------------中断服务程序------------------------------
irqreturn_t key_int(int irq,void *dev_id)			
{
	schedule_work(work1);						
	return 0;
}

//---------------------内核定时器中断服务函数--------------------------
void buttons_timer_function(unsigned long data)  
{	
	key_val = readw(key_base+1)&0b111111;
    switch(key_val)
	{
		case 0b111110 :	
			printk(KERN_WARNING"key_number is key_1\n");break;
		case 0b111101 :	
			printk(KERN_WARNING"key_number is key_2\n");break;
		case 0b111011 :	
			printk(KERN_WARNING"key_number is key_3\n");break;			
		case 0b110111 :	
			printk(KERN_WARNING"key_number is key_4\n");break;
		case 0b101111 :	
			printk(KERN_WARNING"key_number is key_5\n");break;
		case 0b011111 :	
			printk(KERN_WARNING"key_number is key_6\n");break;
		default :
			break;
	}
	//wake_up(&key_que);								//唤醒阻塞
	flag = 1;
	wake_up_interruptible(&key_que);
	printk(KERN_WARNING"Debug information --->>no.3\n");
} 

//---------------------------硬件初始化函数----------------------------
void key_hw_init(void)					
{
	unsigned int data;
	
	data = readl(key_base);
	data &= ~0b11111111	;			//中断模式
	data |=  0b10101010	;
	
	writel(data,key_base);			//写寄存器
}

int key_open(struct inode *node , struct file* filp)//open函数
{
	return 0;
}

ssize_t key_read(struct file *filp, char __user *buffer, size_t size, loff_t *pos)
{
	if(key_val==0b111111)
		flag=0;
	else 
		flag=1;
	
	printk(KERN_WARNING"flag = %d\n",flag);
	
	printk(KERN_WARNING"Debug information --->>no.1\n");
	wait_event_interruptible(key_que,flag);					//无数据就休眠
	printk(KERN_WARNING"Debug information --->>no.2\n");
	
	copy_to_user(buffer,&key_val,4);
	flag=0;													//清空数据
	
	return 4;
}

struct file_operations key_fops =							    //操作函数集
{
	.open = key_open,
	.read = key_read,
};

struct miscdevice key_miscdev = {							    //混杂设备结构
	.minor = 200,
	.name  = "key",
	.fops  = &key_fops,
};

static int __devinit key_probe(struct platform_device *pdev)
{
	int size=0;
	int ret=0;
	ret = misc_register(&key_miscdev);						//注册混杂设备
	
	res_mem = platform_get_resource(pdev,IORESOURCE_MEM,0);
	size = (res_mem->end) - (res_mem->start) + 1;
	key_base = ioremap(res_mem->start,size);
	key_hw_init();											//硬件初始化
	
	//注册中断处理程序
	res_irq = platform_get_resource(pdev,IORESOURCE_IRQ,0);
	
//为方便驱动程序的移植，中断号不再直接指定，改为从设备文件中获取
	request_irq(res_irq->start  ,key_int,IRQF_TRIGGER_FALLING,"key",0);
	request_irq(res_irq->start+1,key_int,IRQF_TRIGGER_FALLING,"key",0);
	request_irq(res_irq->start+2,key_int,IRQF_TRIGGER_FALLING,"key",0);
	request_irq(res_irq->start+3,key_int,IRQF_TRIGGER_FALLING,"key",0);
	request_irq(res_irq->start+4,key_int,IRQF_TRIGGER_FALLING,"key",0);
	request_irq(res_irq->end,    key_int,IRQF_TRIGGER_FALLING,"key",0);
	
	//创建工作队列
	work1 = kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work1, work1_func);
	
	printk(KERN_WARNING"Debug information --->> insmod dirver is OK!!!\n");
	
	//定时器初始化
	init_timer(&buttons_timer);   
    buttons_timer.function  = buttons_timer_function;  
	//向内核注册一个定时器
    add_timer(&buttons_timer);  
	//初始化等待队列
	init_waitqueue_head(&key_que);
	
	return ret;
}

static int __devinit key_remove(struct platform_device *pdev)
{
	misc_deregister(&key_miscdev);						//卸载设备结构

	//注销中断处理程序
	free_irq(S3C_EINT(0),0);							//释放中断
	return 0;
}

struct platform_driver key_driver = {
	.probe   =  key_probe,
	.remove  =   __devexit_p(key_remove),
	.driver  = {
			.name = "my-key",							//名字必须必配
	}
};

static int key_init_1(void)								//内核模块初始化
{	
	platform_driver_register(&key_driver);
	return 0;
}

static void key_exit(void)								    //内核模块卸载
{
	platform_driver_unregister(&key_driver);
}

module_init(key_init_1);		    //初始化模块宏
module_exit(key_exit);			//卸载模块宏
