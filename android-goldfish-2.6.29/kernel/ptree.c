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
		printk("%s[%d] | tgid:%d\n", task->comm, task->pid, task->tgid);
	}
}

void print_pids_dfs(void)
{
	/**
	 * Need to do this non-recursively since kernel stack is limited.
	 * Can do it iteratively by using a stack:
	 * Idea is:
	 * 1) Add root node to a stack list
	 * 2) while stack is not empty do:
	 *        ->Pop an element from the stack
	 *        -> if(element HAS NO CHILDREN)
	 *               print out its details
	 *           else
	 *              add all the children of element to the stack
	 */

	struct task_struct *root_task = &init_task;

}

