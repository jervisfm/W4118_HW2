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

/**
 * Prints out all the process ids in a depth first search *pre-order*
 * traversal.
 *
 * Work in Progress : print_pids+dfs crash due a NULL ptr dereference.
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
	int depth = 0;
	struct tasklist first;

	/**
	 * In Linux head of the linked list should be a
	 * *separate* list_head structure all by it self.
	 */
	struct tasklist *curr_list_item;
	struct task_struct *curr_task;
	struct tasklist stack;
	struct list_head *head;
	INIT_LIST_HEAD(&(stack.list));
	head = &stack.list;
	// initialize first tasklist
	first.task = get_init_process();
	first.depth = 0;

	list_add(&first.list, head);

	if((first.task) != &init_task) {
		printk("Tasks MISTMACT\n");
		printk("init:%p \n first.tsk = %p \n", &init_task, first.task);
		printk("---\n");
		printk("&first.task= %p | &(first.task) %p", &first.task, &(first.task));
	} else {
		printk("Tasks OKAY\n");
		first.task = &init_task;
	}

	// check list size.
	struct list_head *start = head;
	int size = 0;
	struct list_head *curr = head->next;

	for(curr = head->next; curr != head; curr = curr->next) {
		++size;
	}
	printk("\nSize of list is %d\n", size);

	/*
	// debuggin begins here
	curr_list_item  = list_entry(head->next,
					     struct tasklist, list);



	curr_task = curr_list_item->task;
	depth = curr_list_item->depth;

	if(&first == curr_list_item) {
		printk("Address match as expected\n");
	} else {
		printk("Address MISMATCH\n");
		printk("init-task = %p\n", &init_task);
		printk("curr-task = %p\n", curr_task);
	}

	printk("dbg:%s\n", curr_task->comm);

	// debuggin ends here
	*/

	printk("\n===About to start looop===\n");
	int count = 0;
	int MAX = 10;
	while (!list_empty(head) && count++ < MAX) {
		printk("\n******* New LOOP: List size == %d\n", list_size(head));

		curr_list_item  = list_entry(head->next,
					     struct tasklist, list);
		curr_task = curr_list_item->task;
		depth = curr_list_item->depth;

		// remove item from stack.
		list_del(&curr_list_item->list);

		printk("After removal, list size is %d\n", list_size(head));

		 // Let's process it
		print_task(curr_task, depth);

		// Add all children *processes* if any.
		if (has_children(curr_task)) {
			printk("Processing children...\n");
			++depth;
			struct task_struct* temp;
			struct list_head *child_list = &curr_task->children;
			/* we use sibling because a parent's children list
			 * consits of first child's sibling field
			 * this is a consequence of the way list_head works
			 * (its embedding nature).
			 * Other way to see this is that the children are all
			 * siblings of another.
			 * */
			list_for_each_entry(temp, child_list, sibling) {
				if (!is_a_process(temp))
					continue;

				struct tasklist *new = kcalloc(1, sizeof(struct tasklist), GFP_KERNEL);
				new->task = temp;
				list_add(&new->list,head);
				new->depth = depth;
				int proc = is_a_process(temp);
				printk("Adding child proc:%s [%d]. Process:%d\n", temp->comm, temp->pid, proc);
			}

			printk("\n");
		} else {
			printk("No Children found...\n");
		}
	}
	printk("\n==== FINISHED LOOP====");

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
		printk("  ");
	}
	printk("PT : %s\n", task->comm);
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
	/*
	 * init_task is NOT the init process. It is actually
	 * the idle process. Through experimentation I found that
	 * it has a PID value of 0 and only 2 child process, all of
	 * which refer to itself.
	 *
	 * So the correct way to get the init process, is get the
	 * current running process and go up the tree. init usually
	 * has a pid of 1, but I don't do a direct lookup because
	 * it *possible* that the process has a different PID on other
	 * systems.
	 */

	struct task_struct *prev, *curr;
	prev = NULL;
	curr = current;
	for (; curr != &init_task; prev = curr, curr = curr->parent)
		;
	/*
	 * Init process will be just one step below init_task
	 */
	return prev;
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
