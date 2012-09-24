/*
 * ptree.h
 *
 *  Created on: Sep 27, 2012
 */

#ifndef PTREE_H_
#define PTREE_H_

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <asm-generic/errno-base.h>
#include <linux/stddef.h> /* for true and false */
#include <asm/uaccess.h>

/* Stack struct */
struct tasklist {
	struct task_struct *task;
	int depth;
	struct list_head list; /* Linked List */
};


/* acquires lock need to browse task */
void acquire_tasklist_lock(void);

void release_tasklist_lock(void);

int no_children(struct task_struct *task);

int has_children(struct task_struct *task);

struct task_struct *get_next_node(struct task_struct *cur);

void process_node(int idx, struct prinfo *buf, struct task_struct *task);

int dfs_procs(struct prinfo *buf, int *nr);

int has_sibling(struct task_struct *task);
/*
 * Returns the init process which is the root parent
 * of all processes on the system.
 */
struct task_struct *get_init_process(void);

int is_a_process(struct task_struct *task);

#endif /* PTREE_H_ */
