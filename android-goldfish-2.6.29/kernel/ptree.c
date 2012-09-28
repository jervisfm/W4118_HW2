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

	printk("******\n");
	print_pids_dfs();
	//t0();
	//t1();
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
	if (task == NULL)
		return 0;

	struct list_head *children;
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
	if (head == NULL)
		return 0;
	int size = 0;
	struct list_head *curr = head->next;

	for(curr = head->next; curr != head; curr = curr->next) {
		++size;
	}
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
		++depth;
		struct task_struct* temp;
		struct list_head *child_list = &task->children;
		/* Use 'sibling' field because the children of a parent task
		 * are all siblings of another. Thus, parent->children
		 * is a list_head embedded in a task_struct of a child process,
		 *  and it actually references the sibling field.
		 */
		list_for_each_entry(temp, child_list, sibling) {
			if (!is_a_process(temp))
				continue;

			struct tasklist *new = kcalloc(1,
					               sizeof(struct tasklist),
					               GFP_KERNEL);
			new->task = temp;
			list_add(&new->list,head);
			new->depth = depth;
			//int proc = is_a_process(temp);
			//printk("Adding child proc:%s [%d]. Process:%d\n", temp->comm, temp->pid, proc);
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////

/* Test / playing code: To be deleted later */

void t1(void) {
	printk("Going up from current processs...\n");
	struct task_struct *curr;
	for(curr = current; curr != &init_task; curr = curr->parent) {
		printk("[%d] - %s\n", curr->pid, curr->comm);
		printk("No. children == %d\n", list_size(&curr->children));
		printk("No. sibling == %d\n", list_size(&curr->sibling));

		printk("Enumerating children m1...\n");
		struct task_struct *temp;
		list_for_each_entry(temp, &curr->children, sibling) {
			int proc = is_a_process(temp);
			printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
		}

		printk("Enumerating children M2...\n");
		struct list_head *list;
		list_for_each(list, &curr->children) {
			temp = list_entry(list, struct task_struct, sibling);
			int proc = is_a_process(temp);
			printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
		}

		printk("=======Enumerating sibling...\n");
		list_for_each_entry(temp, &curr->sibling, sibling) {
			int proc = is_a_process(temp);
			printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
		}

		printk("\n------\n");
	}

	printk("No. of init children == %d\n", list_size(&curr->children));
	printk("No. of init sibling == %d\n", list_size(&curr->sibling));

}

void t0(void) {
	struct task_struct *t = &init_task;
	struct task_struct *tt = &init_task;

	printk("Init stats:\n");
	printk("No. of init children == %d\n", list_size(&t->children));
	printk("No. of init sibling == %d\n", list_size(&t->sibling));

	tt = list_entry(&t->children, struct task_struct, children);
	printk("[%d] - '%s'\n", tt->pid, tt->comm);
	tt = list_entry(&t->children.next, struct task_struct, children);
	printk("[%d] - '%s'\n", tt->pid, tt->comm);

	printk("Going up from current processs...\n");
	struct task_struct *curr;
	for(curr = current; curr != &init_task; curr = curr->parent) {
		printk("[%d] - %s\n", curr->pid, curr->comm);
		printk("No. children == %d\n", list_size(&curr->children));
		printk("No. sibling == %d\n", list_size(&curr->sibling));

		printk("Enumerating children...\n");
		struct task_struct *temp;
		list_for_each_entry(temp, &curr->children, children) {
			int proc = is_a_process(temp);
			printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
		}

		printk("=======Enumerating sibling...\n");
		list_for_each_entry(temp, &curr->sibling, sibling) {
			int proc = is_a_process(temp);
			printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
		}

		printk("\n------\n");
	}

	struct task_struct *i = get_init_process();
	printk("Attempting to get init process\n");
	printk("[%d] - %s ", i->pid, i->comm);
	printk("No. of init proc children == %d\n", list_size(&i->children));
	printk("**************\n");

	printk("Enumerating init children...");
	struct task_struct *temp;
	list_for_each_entry(temp, &i->children, children) {
		int proc = is_a_process(temp);
		printk("[%d] - %s. Process:%d\n", temp->pid, temp->comm, proc);
	}
	printk("Done\n");
}
