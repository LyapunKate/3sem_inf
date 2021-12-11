#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

//определяем тип файла
char stat_type(unsigned mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:  return 'b';
        case S_IFCHR:  return 'c';
        case S_IFDIR:  return 'd';
        case S_IFIFO:  return 'p';
        case S_IFLNK:  return 'l';   
        case S_IFREG:  return '-';
        case S_IFSOCK: return 's';
        default:       return '?';
    }
}
int main(int argc, char const *argv[])
{	
	if(argc > 1) {
        	printf("Too many arguments\n");
        	printf("Usage: %s\n", argv[0]);
        	return 1;
    	}
	//открываем директорию, обрабатываем ошибку при открытии
	DIR *dir_fd = opendir(".");
	if (dir_fd == NULL) {
		perror("error in opendir");
		return 2;
	}
	//открываем каждый файл директории
	struct dirent *entry;
	while((entry = readdir(dir_fd)) != NULL)
	{  	struct stat st;
	 	if (lstat(entry->d_name, &st) == -1) {
			perror("error in lstat");
			return 3;
		}
	 	else {
		char type = stat_type(st.st_mode);
		}

		printf("%c %s\n", type, entry->d_name);
	}
	
	if(closedir(dir_fd) == -1) {
        	perror("error in closedir");
        	return 4;
    }
	return 0;
}
