#define MAX_COMM 64

struct prinfo {
	long state;			/* current state of process */
	pid_t pid;			/* process id */
	pid_t parent_pid;		/* process id of parent */
	pid_t first_child_pid;  	/* pid of youngest child */
	pid_t next_sibling_pid;  	/* pid of older sibling */
	long uid;			/* user id of process owner */
	char comm[MAX_COMM];		/* name of program executed */
};
