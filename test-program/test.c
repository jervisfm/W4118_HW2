#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../android-goldfish-2.6.29/include/linux/prinfo.h" 

int main()
{
	struct prinfo *buf;
	int nr;
	int nr_returned;

	nr = 1000;
	buf = malloc(sizeof(struct prinfo) * nr);

	nr_returned = syscall(223, buf, &nr);

//	printf("I did not crash i am a god");

	int i;
	for (i = 0; i < nr_returned; i++)
		printf("%s\n", buf[i].comm);
	return 0;
}
