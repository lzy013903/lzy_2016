#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd0a8ddc4, "module_layout" },
	{ 0x7d11c268, "jiffies" },
	{ 0x1298ac43, "mod_timer" },
	{ 0x8949858b, "schedule_work" },
	{ 0xb9e52429, "__wake_up" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x98082893, "__copy_to_user" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x5420d3dd, "platform_driver_register" },
	{ 0x1f3cc41e, "platform_driver_unregister" },
	{ 0xf6d8fc65, "kmalloc_caches" },
	{ 0x27e1a049, "printk" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0x80ab0c30, "add_timer" },
	{ 0x3028e855, "init_timer_key" },
	{ 0x3ec0f0a8, "kmem_cache_alloc_trace" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0xf108648c, "platform_get_resource" },
	{ 0x93d626c3, "misc_register" },
	{ 0x4ad15b1c, "misc_deregister" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "BDE75987D8A121CA81A2ED5");
