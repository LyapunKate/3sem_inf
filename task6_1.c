#include<sys/types.h>
#include<dirent.h>
#include<stdio.h>



char dtype_letter(unsigned d_type)
{
	case DT_BLK: return 'b';
case DT_CHR: return 'c';
case DT_DIR:return 'd';
case DT_FIFO:return 'f';
case DT_LNK:return 'l';
case DT_REG: return 'r';
case DT_SOCK:return 's';
default  :prin 'u';
}
int main(void)
{	
	DIR *dir_fd = opendir(".");
	if (dir_fd == NULL) {
		return 1;
	}
	struct dirent *entry;
	while((entry = readdir(dir_fd)) != NULL)
	{  char type = dtype_letter(entry->dtype);

		printf("%hhu %s\n", entry->d_type, entry->d_name);
	}
	
	closedir(dir_fd);
	return 0;
}
