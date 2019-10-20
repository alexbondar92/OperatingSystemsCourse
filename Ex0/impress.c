#include <linux/kernel.h>

int sys_impress(int id) {
	printk("Look my love %d, I'm a geek :)\n", id);
	return 0;
}