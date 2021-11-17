#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

/* "dprintf" writes all information, that was passed to it, 
 * and "write" can write only part of the information, 
 * that is why in task2_1 we need to do a cycle, 
 * where we check that all the information had been writen. 
 * In this task it isn't necessary to do it*/ 


int main(int argc, char * argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s path text", argv[0]);
		return 1;
	}

	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd < 0)

	{
		perror("Failed to open file for writing");
		return 2;
	}

	if (dprintf(fd, "%s", argv[2]) < 0)
	{
		perror("Failed to write");
		close(fd);
		return 3;
	}

	if (close(fd) < 0)
	{
		perror("Failed during close");
		return 4;
	}

	return 0;
}

