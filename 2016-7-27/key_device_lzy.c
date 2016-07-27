#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");			//使用GPL规范
#define GPNCON 0x7F008830		//控制寄存器

static struct resource key_resource[] = {
	[0] = {
		.start = GPNCON,
		.end   = GPNCON + 8,		//两个寄存器
		.flags = IORESOURCE_MEM,	//资源类型
	},
	[1] = {
		.start = S3C_EINT(0),		//外部中断
		.end   = S3C_EINT(3),		//4个外部中断
		.flags = IORESOURCE_IRQ,	//中断类型
	},
};

struct platform_device key_device = {
	.name		      = "lzy-key",					//设备名字
	.id		          = 0,							//匹配ID
	.num_resources	  = ARRAY_SIZE(key_resource),	//资源数量
	.resource	      = key_resource,				//使用的资源
};

static int button_init(void)
{
    platform_device_register(&key_device);		//注册平台设备
    return 0;
}

static void button_exit(void)
{	   
    platform_device_unregister(&key_device);	//注销平台设备
}

module_init(button_init);
module_exit(button_exit);
