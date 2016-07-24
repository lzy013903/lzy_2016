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
	{ 0x8949858b, "schedule_work" },
	{ 0xf6d8fc65, "kmalloc_caches" },
	{ 0x3ec0f0a8, "kmem_cache_alloc_trace" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0x93d626c3, "misc_register" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0x27e1a049, "printk" },
	{ 0x37a0cba, "kfree" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x4ad15b1c, "misc_deregister" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D42FE594A7CCA149920687E");
