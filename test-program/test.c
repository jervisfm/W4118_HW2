#include <stdio.h>
#include <unistd.h>

int main()
{
	int n = syscall(223, NULL, NULL);
	printf("Hello World\n");
	printf("%d\n", n);
	return 0;
}
