#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>

void print_cpu_time()
{
	struct rusage usage;
	getrusage (RUSAGE_SELF, &usage);
	printf ("CPU time: %ld.%06ld sec user, %ld.%06ld sec system\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec, usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
}

/*Prints the argument list, one argument to a line, 
 * of the process given by PID. */

void print_process_arg_list (pid_t pid)
{
	int fd;
	char filename[24];
	char arg_list[1024];
	size_t length;
	char* next_arg;

	/* Generate the name of the cmdline file fro the process */

	snprintf (filename, sizeof(filename), "/proc/%d/cmdline", (int)pid);

	/*Read the contents of the file. */

	fd = open(filename, O_RDONLY);
	length = read (fd, arg_list, sizeof (arg_list));
	close (fd);

	/* Add ending symbol */

	arg_list[length] = '\0';

	/* Loop over arguments. Arguments re separated by NULLs. */

	next_arg = arg_list;

	while (next_arg < arg_list +length) {
		/* Print the argument. Each is NUL-terminated, 
		 * that is why we can separate them as a string */
		printf ("%s\n", next_arg);
		/* As all arguments are NULL-separated,
		 * the length of next_argument is not the length of the string
		 * so we need to count the lenghth of the string */
		next_arg += strlen (next_arg) + 1;
	}
}


int main(int argc, char* argv[])
{
	printf("The process ID is %d\n", (int) getpid());
	printf("The parent process is %d\n", (int) getppid());
	pid_t pid = (pid_t) atoi (argv[1]);
	print_process_arg_list (pid);
	print_cpu_time();

	return 0;

}
