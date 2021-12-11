#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

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

char * rights (int mode)
{
	char * mode_rights = malloc (10*sizeof(char));
	int i = 8;
	for (i = 8; i >= 0; i--)
		mode_rights[8-i] = (mode & (1 << i) ? "xwr"[i%3]: '-');
	mode_rights[9] = '\0';
	return mode_rights;
}

int main(int argc, char const *argv[])
{	
	if(argc > 2) {
        	printf("Too many arguments\n");
        	printf("Usage: %s\n", argv[0]);
        	return 1;
    	}

	if (argc == 2) {
		if (chdir(argv[1]) == -1) {
				perror("chdir");
				return 2;
				}
			}

	//открываем директорию, обрабатываем ошибку при открытии
	DIR *dir_fd = opendir(".");

	if (dir_fd == NULL) {
		perror("error in opendir");
		return 3;
	}

	//открываем каждый файл директории
	struct dirent *entry;
	while((entry = readdir(dir_fd)) != NULL)
	{  	struct stat st;
	 	if (lstat(entry->d_name, &st) == -1) {
			perror("error in lstat");
			return 4;
		}
	 	else {
			char type = stat_type(st.st_mode);
			printf("%c %s %s\n", type, rights(st.st_mode & ALLPERMS), entry->d_name);
		}
	}
	
	if(closedir(dir_fd) == -1) {
        	perror("error in closedir");
        	return 4;
    }
	return 0;
}
