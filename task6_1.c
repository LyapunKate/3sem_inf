#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

char dirent_type(unsigned d_type) {
    switch (d_type) {
        case DT_BLK:  return 'b';
        case DT_CHR:  return 'c';
        case DT_DIR:  return 'd';
        case DT_FIFO:  return 'p';
        case DT_LNK:  return 'l';
        case DT_REG:  return '-';
        case DT_SOCK: return 's';
        default:       return '?';
    }
}

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
	while(1)
	{
		errno = 0;
		struct dirent *entry = readdir(dir_fd);
		if (entry == NULL) {
			if (errno == 0)
				break;
			perror("readdir");
			closedir(dir_fd);
			return 3;
		}

		char entry_type = dirent_type(entry->d_type);
		
		if (entry_type == '?') {

			struct stat st;
			if (lstat(entry->d_name, &st) == 0) {
				entry_type = stat_type(st.st_mode);
				}
			else {
				perror("error in lstat");
				entry_type = "!";
			}
		}
		printf("%c %s\n", entry_type, entry->d_name);
		
	}
	
	if(closedir(dir_fd) == -1) {
        	perror("error in closedir");
        	return 5;
    }
	return 0;
}
