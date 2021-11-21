#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

//семафор mutex нужен для организации взаимоисключения на критических участках
//

typedef struct {
	pthread_mutex_t mutex;
	unsigned long long counter;
	int iterations;
} work_area_t;

void * thr_body(void * arg) {
	work_area_t * w = arg;
	for (int i = 0; i < w->iterations; i++) {
		pthread_mutex_lock(&w->mutex);
		w->counter++;
		pthread_mutex_unlock(&w->mutex);
	}

	puts("Done (secondary)");
	return NULL;
}

int main(int argc, char * argv[]) {
	
	//PTHREAD_MUTEX_INITIALIZER - создание mutex'a с "базовыми настройками"
	work_area_t data = {
		.mutex = PTHREAD_MUTEX_INITIALIZER,
		.counter = 0,
		.iterations = (argc > 1) ? atoi(argv[1]) : 1000,
	};

	pthread_t secondary_thread_id;
	//в случае ошибки возвращает положительное значение, а не отрицательное
	//как остальные функции
	if (errno = pthread_create(&secondary_thread_id, NULL, thr_body, &data)) {
		perror("pthread_create");
		return 1;
	}
	for (int i = 0; i < 10; i++) {
		pthread_mutex_lock(&data.mutex);
		data.counter++;
		pthread_mutex_unlock(&data.mutex);
	}
	puts("Done (main)");
	//нить исполнения, вызвавшая эту функцию переходит в состояние ожидания
	//до завершения указанного thread'a
	pthread_join(secondary_thread_id, NULL);
	return 0;
}

