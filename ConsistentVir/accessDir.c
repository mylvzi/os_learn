#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main()
{
	DIR* dp = opendir(".");
	struct dirent* d;
	while((d = readdir(dp)) != NULL)
	{
		printf("%d %s\n", (int)d->d_ino, (int*)d->d_name);
	}	
	closedir(dp);
	return 0;
}
