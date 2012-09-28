/*
 * ptree.c
 *
 *  Created on: Sep 27, 2012
 */

#include "ptree.h"
#include <linux/stddef.h> /* for true and false */

/**
 * Defines the New System Call. You can find the corresponding
 * declaration in linux/syscalls.h
 */
SYSCALL_DEFINE2(ptree, struct prinfo, *buf, int, *nr)
{
	printk("Hello world from kernel\n");
	printk("Listing all processes...\n");
	print_all_pids();

	printk("******\n");
	print_pids_dfs();
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
		int proc = is_a_process(task);
		printk("%s[%d] | tgid:%d Process:%d\n", task->comm, task->pid, task->tgid, proc);
	}

}


/**
 * Prints out all the process ids in a depth first search *pre-order*
 * traversal.
 *
 */
void print_pids_dfs(void)
{
	/**
	 * Need to do this non-recursively since kernel stack is limited.
	 * Can do it iteratively by using a stack data structure.
	*/
	int depth = 0; /* depth of a process */
	struct tasklist stack;
	struct tasklist first; /* first data item on stack */
	struct tasklist *curr_list_item; /* an item on the stack */
	struct task_struct *curr_task;
	struct list_head *head; /* head of stack list*/

	/* Initialize Stack */
	INIT_LIST_HEAD(&(stack.list));
	head = &stack.list;

	/* initialize first tasklist and add it to the stack */
	first.task = get_init_process();
	first.depth = 0;
	list_add(&first.list, head);

	while (!list_empty(head)) {
		/*Pop top item off the stack*/
		curr_list_item  = list_entry(head->next,
					     struct tasklist, list);
		curr_task = curr_list_item->task;
		list_del(&curr_list_item->list);

		/* Process current task */
		depth = curr_list_item->depth;
		print_task(curr_task, depth);

		 /* Add all children *processes* to the stack */
		add_all_children_processes(depth, head, curr_task);
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
 * If task is NULL, it returns 0
 */
int no_children(struct task_struct *task)
{
	struct list_head *children;
	if (task == NULL)
		return 0;
	children = &task->children;
	if (list_empty(children)) {
		return true;
	} else {
		return false;
	}
}
/*
 * Prints the task struct.
 */
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
 * If task is NULL, it returns 0.
 */
int has_children(struct task_struct *task)
{
	if (task == NULL)
		return 0;

	return !no_children(task);
}

/**
 * Determines the size of the given list
 */
int list_size(struct list_head *head) {
	int size = 0;
	struct list_head *curr;

	if (head == NULL)
		return 0;

	for (curr = head->next; curr != head; curr = curr->next)
		++size;
	return size;
}

/*
 * Returns the init process task struct.
 */
struct task_struct* get_init_process(void) {
	return &init_task;
}

/**
 * Determines if the given task struct is a process or not.
 * Return 1 if true and 0 if false
 */
int is_a_process(struct task_struct *task) {
	if (thread_group_empty(task))
		return true; /* A process with 0 threads */
	else  {
		if (thread_group_leader(task))
			return true; /* A process with at least 1 thread */
		else
			return false;
	}
	return false;
}

int add_all_children_processes(int depth, struct list_head *head,
			       struct task_struct *task) {
	if (head == NULL || task == NULL)
		return 0;

	if (has_children(task)) {
		struct task_struct* temp;
		struct list_head *child_list = &task->children;
		++depth;
		/* Use 'sibling' field because the children of a parent task
		 * are all siblings of another. Thus, parent->children
		 * is a list_head embedded in a task_struct of a child process,
		 *  and it actually references the sibling field.
		 */
		list_for_each_entry(temp, child_list, sibling) {
			struct tasklist *new;
			if (!is_a_process(temp))
				continue;

			new = kcalloc(1, sizeof(struct tasklist), GFP_KERNEL);
			new->task = temp;
			list_add(&new->list,head);
			new->depth = depth;
		}
	}
	return true;
}
