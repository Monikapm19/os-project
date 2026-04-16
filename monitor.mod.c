#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x7f79e79a, "kthread_create_on_node" },
	{ 0x630dad60, "wake_up_process" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0xfaabfe5e, "kmalloc_caches" },
	{ 0xc064623f, "__kmalloc_cache_noprof" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x0571dc46, "kthread_stop" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x4e54d6ac, "cdev_del" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0x5e505530, "kthread_should_stop" },
	{ 0xc13edbcb, "find_vpid" },
	{ 0x848a0d8d, "pid_task" },
	{ 0x8ffd462b, "send_sig" },
	{ 0x67628f51, "msleep" },
	{ 0xd272d446, "__fentry__" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xd5f66efd, "cdev_init" },
	{ 0x8ea73856, "cdev_add" },
	{ 0xbebe66ff, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x7f79e79a,
	0x630dad60,
	0xe8213e80,
	0xd272d446,
	0xbd03ed67,
	0x092a35a2,
	0xbd03ed67,
	0xfaabfe5e,
	0xc064623f,
	0xd272d446,
	0x0571dc46,
	0xcb8b6ec6,
	0x4e54d6ac,
	0x0bc5fb0d,
	0x5e505530,
	0xc13edbcb,
	0x848a0d8d,
	0x8ffd462b,
	0x67628f51,
	0xd272d446,
	0x9f222e1e,
	0xd5f66efd,
	0x8ea73856,
	0xbebe66ff,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"kthread_create_on_node\0"
	"wake_up_process\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"__ref_stack_chk_guard\0"
	"_copy_from_user\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"__stack_chk_fail\0"
	"kthread_stop\0"
	"kfree\0"
	"cdev_del\0"
	"unregister_chrdev_region\0"
	"kthread_should_stop\0"
	"find_vpid\0"
	"pid_task\0"
	"send_sig\0"
	"msleep\0"
	"__fentry__\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"cdev_add\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "87E1CCE0C31835500CEBB99");
