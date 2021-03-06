//программа порождает дочерний процесс, ждёт его завершения
//выводит информацию о завершении
//
//Функция waitpid приостанавливает выполнение текущего процесса до тех пор, 
//пока не появится сигнал, который завершит текущий процесс, 
//либо потребует вызвать функцию обработчик
//Она используется для ожидания изменения состояния процесса-потомка
//pid_t waitpid(pid_t pid, int *status, int options);
//с помощью options задаём, чтобы он ожидал не только прекращения работы процесса

#define _GNU_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>


int main (void) {
	pid_t pid;
	// порождаем дочерний процесс
	// выходим в случае ошибки
	if ((pid = fork()) < 0) {
		perror("Can't fork child");
		return 1;
	}
	//pid == 0 при вызове pid = fork(), когда это дочерний процесс
	if (pid == 0) {
		pid_t child = getpid();
		printf("This is child process PID = %d\n", child);
		sleep(10);
	}
	//это в родительском процессе
	else {
		while(1) {
                    int status = 0;
		    //узнаём статус и идентификатор завершившегося процесса
       		    if ((waitpid(pid, &status, WUNTRACED | WCONTINUED)) < 0) {
                    // печатаем в случае ошибки и продолжаем работу
                        perror("Some error on waitpid");
                        break;
		    }
			
		    //WIFEXITED возвращает истинное значение, если потомок нормально завершился
                    if(WIFEXITED(status)) {
                            printf("Child process PID %d exited with code %d\n", pid, WEXITSTATUS(status));
			    break;
                    }
                    //WIFSIGNALED возвращает истинное значение, если потомок завершился из-за сигнала
                    if(WIFSIGNALED(status)) {
                            int child_signal = WTERMSIG(status);
                            printf("Child process PID %d was killed by signal %d (%s)", pid, child_signal, strsignal(child_signal));
			    if (WCOREDUMP(status))
				    puts("With core dump\n");
			    break;
                    }

		    //возвращает истинное значение, если потомок остановлен по сигналу
		    if(WIFSTOPPED(status)) {
			    printf("Child process PID %d was stopped by signal %d (%s)\n", pid, WSTOPSIG(status), strsignal(WSTOPSIG(status)));
			    continue;
		    }
			//возвращает истинное значение, если потомок продолжил работу по сигналу SIGCONT
		    if(WIFCONTINUED(status)) {
			    printf("Child process PID %d continue to work\n", pid);
			    continue;
		    }


		}
	}
	return 0;
}


