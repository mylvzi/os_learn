#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = open("foo",O_CREAT | O_WRONLY | O_TRUNC);
	printf("%d\n", fd);
	return 0;
}
