/*
 * ptree.c
 *
 *  Created on: Sep 27, 2012
 */

#include <linux/syscalls.h>

/* A do nothing test function */
int do_nothing(void) {
	return 0;
}


SYSCALL_DEFINE2(ptree, struct prinfo, *buf, int, *nr)
{
	printk("Hello world from kernel");
	return 789;
}
