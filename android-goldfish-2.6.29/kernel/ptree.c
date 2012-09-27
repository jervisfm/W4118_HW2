/*
 * ptree.c
 *
 *  Created on: Sep 27, 2012
 */

#include <linux/syscalls.h>
#include <linux/sched.h>

#include "ptree.h"

/* A do nothing test function */
int do_nothing(void)
{
	return 0;
}

/**
 * Defines the New System Call. You can find the corresponding
 * declaration in linux/syscalls.h
 */
SYSCALL_DEFINE2(ptree, struct prinfo, *buf, int, *nr)
{
	printk("Hello world from kernel\n");
	printk("Listing all processes...\n");
	print_all_pids();
	return 888;
}


/*
 * Test function:
 * Prints pids, of every single running process.
 */
void print_all_pids(void)
{
	struct task_struct *task;
	for_each_process(task) {
		printk("%s[%d]\n", task->comm, task->pid);
	}
}
