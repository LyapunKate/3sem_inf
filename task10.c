#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#define BUF_SIZE 4096

/* прочитаем все доступные события
*/

void handle_event(int fd, int wd, char* pathname) {
	char buf[BUF_SIZE];
	const struct inotify_event *event;
	ssize_t len;
	char *ptr;
	for(;;) {
		len = read(fd, buf, sizeof(buf));
		//когда read не видит событий для чтения он возвращает -1 и 
		//в errno устанавливает EAGAIN
		//в этом случае мы выходим из цикла
		//если errno не равен EAGAIN, а len = -1, то ошибка в read, 
		//ссообщаем об этом
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		if (len <= 0) {
			break;
		}
		
		//перебираем все србытия из буфера
		//в буфер мы их считали на предыдущем шаге
		for(ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event -> len) {
			//печатаем информацию о типе события
			//нам нужно отслеживать появление новых файлов
			//значит нас интересует создание, перемещение и переименование
			//так как при переименовании появляется событие 
			//IN_MOVED_FROM, для директории, из которой был перемещён переименованный файл, 
			//(если кроме переименования происходило также перемещение, rename позволяет сделать это), 
			//событие IN_MOVED_TO для директории, в которую был перемещён, и 
			//IN_MOVE_SELF для самого файла
			//IN_MOVE = IN_MOVED_TO | IN_MOVED_FROM
			event = (const struct inotify_event*)buf;
			if(event -> mask & IN_CREATE) {
				printf("IN_CREATE: ");
			}
			if((event->cookie != 0) && (event -> mask & IN_MOVE)) {
				printf("IN_MOVE (rename): ");
			
			}

			if (wd == event -> wd) {
				printf("%s/", pathname);
			}
			if (event->len) {
				if ((event -> name) == NULL)
				       printf("?");
				else
					printf("%s", event->name);
			}

			if (event-> mask & IN_ISDIR) {
				printf(" [directory]\n");
			} else {
				printf(" [file]\n");
			}
		}
	}
}

int main (int argc, char* argv[]) {
	if (argc != 2) {
		perror("argc");
		exit(EXIT_FAILURE);
	}

	char buf;
	int fd, poll_num, wd;

	fd = inotify_init1(IN_NONBLOCK);
	if(fd == -1) {
		perror("inotify_init1");
		exit(EXIT_FAILURE);
	}

	//смотрим на argv[1] на предмет появления новых файлов
	wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MOVE);
	if (wd == -1) {
		perror("inotify_add_watch");
		exit(EXIT_FAILURE);
	}
	close(fd);
	return 0;
}

