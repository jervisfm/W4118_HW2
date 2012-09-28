/*
 * ptree.c
 *
 *  Created on: Sep 27, 2012
 */


#include "ptree.h"
#include <linux/stddef.h> /* for true and false */

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

/**
 * Prints out all the process ids in a depth first search *pre-order*
 * traversal.
 */
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


	/*
	 * Initialize Stack
	 */
	struct tasklist stack;
	INIT_LIST_HEAD(&stack.list);
	struct list_head *head = &stack.list;
	struct tasklist first = {
			.depth = 0,
			.task = &init_task,
	};
	list_add(&first.list, head);
	struct tasklist *curr_list_item;
	struct task_struct *curr_task;
	int depth = 0;
	while (!list_empty(&stack.list)) {
		curr_list_item  = list_entry(stack.list.next,
					     struct tasklist, list);
		curr_task = curr_list_item->task;
		depth = curr_list_item->depth;

		// remove item from stack.
		list_del(&curr_list_item->list);

		 /* Let's process it */
		print_task(curr_task, depth);

		// Add all children if any
		if (has_children(curr_task)) {
			++depth;
			struct tasklist* temp;
			struct list_head *child_list = &curr_task->children;
			list_for_each_entry_reverse(temp, child_list, list) {
				struct tasklist *new = kcalloc(1, sizeof(struct tasklist), GFP_KERNEL);
				new->task = temp;
				list_add(&new->list,head);
			}
			temp->depth = depth;
		}
	}
}

void acquire_tasklist_lock(void)
{
	read_lock(&tasklist_lock);
}

void release_tasklist_lock(void)
{
	read_unlock(&tasklist_lock);
}

/**
 * Determines if the task has no children tasks.
 * Returns 1 if this is true (i.e. no children)
 * and 0 otherwise.
 */
int no_children(struct task_struct *task)
{
	struct list_head *children = &task->children;
	if (list_empty(children)) {
		return true;
	} else {
		return false;
	}
}

void print_task(struct task_struct *task, int depth)
{
	int i = 0;
	for (i = 0; i < depth; ++i) {
		printk("\t");
	}
	printk("%s\n", task->comm);
}

/**
 * Determines if the task has child tasks.
 * Returns 1 if this is true (i.e. no children)
 * and 0 otherwise.
 */
int has_children(struct task_struct *task)
{
	return !no_children(task);
}
