#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init my_init(void)
{
	printk(KERN_INFO "Hello world!\n");
	return 0;
}

static void __exit my_exit(void)
{
	printk(KERN_ALERT "Goodbye world!\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ECEN449 student (and others)");
MODULE_DESCRIPTION("Simple Hello World Module");

module_init(my_init);
module_exit(my_exit);
