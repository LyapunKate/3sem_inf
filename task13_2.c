#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	pid_t pid;
	
	pid = fork();
	switch(pid) {
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			printf("child\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(getpid()));
			sleep(3);
			printf("child process finished\n");
			exit(0);
		default:
			printf("Parent\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(getpid()));
			printf("parent process finished\n");
			exit(0);
	}
	return 0;
}

