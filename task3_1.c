#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

ssize_t writeall (int fd, const void *buf, size_t count)
{
	size_t bytes_written = 0;
	const uint8_t * buf_addr = buf;
	while (bytes_written < count) {
		ssize_t res = write (fd, buf_addr + bytes_written, count - bytes_written);
		if (res < 0) {
			return res;
		}
		bytes_written += res;
	}
	return (ssize_t)bytes_written;
}

int copy_file (int file_to_copy, int copy_of_the_file)
{
	char * read_inf;

	read_inf = (char *)malloc(4096);
	if (read_inf == NULL)
	{
		perror("Failed memory allocation");
		return 6;
	}

	int check;

	while ((check = read(file_to_copy, read_inf, 4096)) != 0)
	{
		if (check < 0)
		{
			perror("File reading error");
			return 7;
		}

		if (writeall(copy_of_the_file, read_inf, strlen(read_inf)) < 0)
		{
			perror("File writing error");
			return 8;
		}
	}

	free(read_inf);
	return 0;
}


int main (int argc, char const *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s less or more arguments", argv[0]);
		return 1;
	}

	struct stat st;
	if (lstat(argv[1], &st) == -1) {
		perror("lstat failed");
		return 2;
	}

	if (((st.st_mode) & (S_IFMT)) != S_IFREG) {
		perror ("Not a regular file");
		return 3;
	}

	int file_to_copy = open (argv[1], O_RDONLY);
	if (file_to_copy < 0) {
		perror("failed to open read-file");
		return 4;
	}

	int copy_of_the_file = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (copy_of_the_file < 0) {
		perror("failed to open copy-file");
		return 5;
	}

	int result = copy_file(file_to_copy, copy_of_the_file);

	if(close(file_to_copy) < 0) {
		perror("failed to close read-file");
		return 9;
	}

	if(close(copy_of_the_file) < 0) {
		perror("failed to close copy-file");
		return 10;
	}

	return 0;
}










