#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>



int main(int argc, char* argv[]) {

	mqd_t queue;
	char* queue_name;

	if (argc != 2) {
		printf("Usage %s /queue_name\n", argv[0]);
	return 1;
	}
	queue_name = argv[1];

// Открываем очередь с правами на посылку и получение сообщений - O_RDWR или создаём(если не существует ещё) - для этого O_CREAT
	queue = mq_open(queue_name, O_RDWR | O_CREAT, 0622, NULL);
	if (queue == (mqd_t) -1) {
		perror("mq_open");
	return 1;
	}

//Получаем информацию про очередь
	struct mq_attr q_inf;
	mq_getattr(queue, &q_inf);
	//to do: add error

	long mq_msgsize = q_inf.mq_msgsize; //переменная, отвечающую за верхний предел размера сообщений, которые могут быть помещены в очередь
	char buf[mq_msgsize + 1];

	while (1) {

	size_t message = (size_t)mq_receive(queue, buf, (size_t)mq_msgsize, NULL); //количество байтов полученного сообщения
	if (message == (size_t) -1) { //Проверка ошибки
		perror("mq_receive"); 
	break;
	}

	 buf[message] = '\0';
	 printf("%s\n", buf);
	}  
	
	// cleanup:
	mq_unlink(argv[1]); //удаляет очередь сообщений
	mq_close(queue); //удаляет связь между дескриптором очереди сообщений, mqdes, и его очередью сообщений
	
	return 0;
}
