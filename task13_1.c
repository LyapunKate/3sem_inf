#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// функция обрабатывает сигнал SIGCHILD 
void my_handler(int nsig) {
	int status;
	pid_t pid;
	//узнаём статус и идентификатор завершившегося процесса
	if ((pid = waitpid(-1, &status, 0)) < 0) {
		// печатаем в случае ошибки и продолжаем работу
		printf ("Some error on waitpid errno = %d\n", errno);
	}
	else {
		// если всё хорошо, то анализируем статус завершившегося процесса
		if ((status & 0xff) == 0) {
			// процесс заверщился с вызовом функции exit
			printf("Process %d was exited with status %d\n", pid, status >> 8);
	}
		else if ((status & 0xff00) == 0) {
			// процесс завершён с помощью сигнала
		       printf("Process %d killed by signal %d %s\n", pid, status &0x7f, (status & 0x80) ?
			       "with core file" : "without core file");
		}
	}
}


int main (void) {
	pid_t pid, ppid;
	(void) signal (SIGCHLD, my_handler);
	// порождаем дочерний процесс
	if ((pid = fork()) < 0) {
		printf("Can\'t fork child 1\n");
		exit(1);
	}
	else if (pid == 0) {
		exit(100);
		// дочерний процесс завершается с кодом 200
	}

	sleep (5000);
	return 0;
}	


