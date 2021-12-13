//скопировать данные из одного файла в другой
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#define buf_size 1048576

ssize_t writeall (int fd, const void *buf, size_t count)
{
	size_t bytes_written = 0;
	const uint8_t * buf_addr = buf;
	while (bytes_written < count) {
		ssize_t res = write (fd, buf_addr + bytes_written, count - bytes_written);
		if (res < 0) {
			return res;
		}
		bytes_written += (size_t)res;
	}
	return (ssize_t)bytes_written;
}

int copy_file (int file_to_copy, int copy_of_the_file)
{
	char * read_inf;

	read_inf = (char *)malloc(buf_size);
	if (read_inf == NULL)
	{
		perror("Failed memory allocation");
		return 6;
	}

	ssize_t bytes_read;
	int result = 0;
	//считываем информацию, функция read возвращает количество прочитанных байтов
	while ((bytes_read = read(file_to_copy, read_inf, buf_size)) > 0)
	{
		//записываем считанную информацию
		//в writeall передаём куда писать, что и сколько байт (результат функции read)
		//проверяем на ошибку
		if (writeall(copy_of_the_file, read_inf, (size_t)bytes_read) < 0)
		{
			perror("File writing error");
			result = 8;
			break;
		}
	}
	//проверяем, что не было ошибки при считывании
	if (bytes_read < 0)
	{
		perror("File reading error");
		result = 7;
	}

	free(read_inf);
	return result;
}


int main (int argc, char const *argv[]) {
	//проверяем, что передали нужное количество аргументов (что писать и куда)
	if (argc != 3) {
		fprintf(stderr, "Usage: %s less or more arguments", argv[0]);
		return 1;
	}
	//проверяем на ошибку при открытии
	int file_to_copy = open (argv[1], O_RDONLY);
	if (file_to_copy < 0) {
		perror("failed to open read-file");
		return 4;
	}
	//0644 - -rw-r--r-- 
	//пользователи -- чтение и запись, группы -- только чтение, остальные -- только чтение
	//(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH), 
	
	int copy_of_the_file = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (copy_of_the_file < 0) {
		perror("failed to open copy-file");
		return 5;
	}
	//вызываем функцию копирования
	int result = copy_file(file_to_copy, copy_of_the_file);
	//проверяем на ошибку при закрытии файлов
	if(close(file_to_copy) < 0) {
		perror("failed to close read-file");
		result = 9;
	}

	if(close(copy_of_the_file) < 0) {
		perror("failed to close copy-file");
		result = 10;
	}

	return result;
}
