#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../android-goldfish-2.6.29/include/linux/prinfo.h" 
#include "test.h"

int main(int argc, char** argv)
{
	struct prinfo *buf;
	int nr;
	int rc;

	/* Initialize NR value. */
	if (argc <= 1) /* no parameters passed */ {		
		nr = DEFAULT_NR_ARG; 	}
	else {
		if (is_number(argv[1]))
			nr = atoi(argv[1]);
		else
			nr = DEFAULT_NR_ARG;	
	}

	buf = calloc(nr, sizeof(struct prinfo));
	if (buf == NULL) {
		printf("Could not allocate buffer to store processes info\n");
		exit(-1);
	}

	rc = syscall(223, buf, &nr);

	if (rc < 0) {
		perror("ptree");
		return -1;
	}

	print_tree(buf, nr);

	printf("\nTotal entries: %d\n", rc);
	return 0;
}

void print_tree(struct prinfo *tree, const int size){
	int id_stack[size];
	id_stack[0] = 0;
	int num_tabs = 0;
	int i = 0;
	for(i = 0; i < size; i++){
		while(tree[i].parent_pid != id_stack[num_tabs]){
			num_tabs--;
		}
		print_prinfo(num_tabs, tree[i]);
		num_tabs++;
		id_stack[num_tabs] = tree[i].pid;
	}
	
}

void print_prinfo(int count, struct prinfo p){
	char returned[count+1];
	int i = 0;
	for(i=0; i<count; i++)
		returned[i] = '\t'; 
	returned[count] = '\0';
	printf("%s %s,%d,%ld,%d,%d,%d,%ld\n", returned, p.comm, p.pid, p.state,
			p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);

}

int is_number(char *string)
{
	int i = 0; 

	if (string == NULL)
		return 0;

	for (; string[i] != '\0'; ++i) {
		if (!isdigit(string[i])) {
			return 0;
		}
	}
	return 1;
}
