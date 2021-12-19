#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

void PrintInfo(mqd_t queue) {
	struct mq_attr q_inf;
	//error EBADF если пихаем что-то не то
	if (mq_getattr(queue, &q_inf) == -1) {
        	perror("mq_getattr");
		exit(EXIT_FAILURE); 
    	}
    	printf("queue descriptor:          %d\n", queue);
    	printf("queue flags :              %ld\n", q_inf.mq_flags);
    	printf("Max message amount :       %ld\n", q_inf.mq_maxmsg);
    	printf("Max message size (bytes) : %ld\n", q_inf.mq_msgsize);
   	 printf("Current message amount :   %ld\n", q_inf.mq_curmsgs);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage %s /queue_name\n", argv[0]);
        return 1;
    }

    // Создаём очередь
    mqd_t queue = mq_open(argv[1], O_RDWR | O_CREAT, 0600, NULL); 
    //создает новую очередь сообщений POSIX или открывает существующую очередь
    //возвращает дескриптор очереди сообщений для использования другими функциями очереди сообщений
    if (queue == (mqd_t) -1) {
        perror("mq_open");
       	//В случае ошибки, mq_open() возвращает (mqd_t) -1
        return 1;
    }

    PrintInfo(queue);

    // Вывод сообщения
    if (mq_send(queue, "hello", strlen("hello"), 0) == -1) { 
	    //добавляет сообщение, на которое указывает msg_ptr, в очередь сообщений, на которую ссылается дескриптор mqdes
        perror("mq_send");
    } 

    PrintInfo(queue);
    
    struct mq_attr q_inf;
    if (mq_getattr(queue, &q_inf) == -1) {
             perror("mq_getattr");
    }


    char* buf = (char*) malloc((size_t)q_inf.mq_msgsize);
    if (mq_receive(queue, buf, (size_t)q_inf.mq_msgsize, NULL) == -1) { 
	    //удаляет самое старое сообщение с наивысшим приоритетом из очереди сообщений, на которую ссылается дескриптор mqdes, и помещает его в буфер, на который указывает msg_ptr
        perror("mq_receive");
        free(buf);
        return -1;
    }

    PrintInfo(queue);

    // поскольку очередь -- глобальный объект, ее закрытие и даже завершение программы 
    // не приводит к удалению
    mq_unlink(argv[1]); //удаляет очередь сообщений
    // cleanup:
    mq_close(queue); //удаляет связь между дескриптором очереди сообщений, mqdes, и его очередью сообщений
    free(buf);
    return 0;
}
/*
struct mq_attr {
    long mq_flags;        Flags: 0 or O_NONBLOCK 
    long mq_maxmsg;       Max. # of messages on queue 
    long mq_msgsize;      Max. message size (bytes) 
    long mq_curmsgs;      # of messages currently in queue 
};*/
