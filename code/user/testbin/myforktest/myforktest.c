#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

int main()
{
	int status;
	int pid;
	int childpid;
	int parent;
	int returnCode=fork();

	if(returnCode==0)
	{
		childpid=getpid();
		printf("I am child:%d\n",childpid);
		return -10;
	}
	else
	{
		//printf("Return:%d",returnCode);
		//printf("Wait for child\n");
		parent=getpid();
		printf("I am parent:%d\n",parent);
		pid=waitpid(returnCode,&status,0);
		printf("Child id returned" "%d",pid);
		printf(" Return code: %d\n",status);

	}
	return 0;
}
