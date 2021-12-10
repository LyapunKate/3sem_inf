#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <string.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <grp.h>
#include <pwd.h>

int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);

// эта функция выводит дату и время с миллисекундами
static void print_time(const char *type, struct statx_timestamp *ts) {

	time_t time;
	char buffer[20];
	//если меньше, то время (часы, минуты, секунды) не помещается

	time = ts->tv_sec;
	struct tm *tm = localtime(&time);
	strftime(buffer, sizeof(buffer), "%F %T", tm);
	printf ("%s %s.%09u ", type, buffer, ts->tv_nsec);
 	strftime(buffer, sizeof(buffer), "%z", tm);
	printf("%s\n", buffer);
}	

//определяем тип файла
char * filetype (int mode)
{
	switch (mode & S_IFMT) {
                case S_IFBLK: return("block device\n");
                case S_IFCHR: return("character device\n");
                case S_IFDIR: return("directory\n");
                case S_IFIFO: return("FIFO/pipe\n");
                case S_IFLNK: return("symlink\n");
                case S_IFREG: return("regular file\n");
                case S_IFSOCK: return("socket\n");
                default: return("unknown?\n");
	}
}

//определяем права на редактирование и тд
char * rights (int mode)
{
	char * mode_rights = malloc (10*sizeof(char));
	int i = 8;
	for (i = 8; i >= 0; i--)
	       mode_rights[8-i] = (mode & (1 << i) ? "xwr"[i%3]: '-');
	mode_rights[9] = '\0';
	return mode_rights;
}
// выводим имя пользователя латиницей
char * user_name(uid_t uid) {
	struct passwd *info;
	info = getpwuid(uid);

	return (info == NULL) ? NULL : info -> pw_name;
}
//выводим имя группы латиницей
char * group_name(uid_t uid) {
	struct group *info;
	info = getgrgid(uid);

	return (info == NULL) ? NULL : info -> gr_name;
}


int main(int argc, char *argv[])
{
	struct stat sb; 
	struct statx stx; //нужно для вывода времени с миллисекундами
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (lstat(argv[1], &sb) == -1) {
		perror("lstat");
		exit(EXIT_FAILURE);
	}
	
	statx(AT_FDCWD, argv[1], AT_STATX_SYNC_AS_STAT, STATX_ALL, &stx);
	//здесь используем данные lstat
	printf("File name %s\n", argv[1]);
	printf("ID of containing device: [%lxh, %ldd]\n", (long) (sb.st_dev), (long) (sb.st_dev));
	fputs("File type:		", stdout);
	printf("%s", filetype(sb.st_mode));
	printf("I-node number: %ld\n", (long) sb.st_ino);
	printf("Mode_rights: (%04o/%s)\n", sb.st_mode & ALLPERMS, rights(sb.st_mode & ALLPERMS));
	printf("Link count: %ld\n", (long) sb.st_nlink);
	printf("Ownership UID=%ld/\t%s   GID=%ld/\t%s\n", (long) sb.st_uid, user_name(sb.st_uid), (long) sb.st_gid, group_name(sb.st_gid));
	printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
	printf("File size: %lld bytes\n", (long long) sb.st_size);
	printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);

	//здесь используем данные statx
	print_time("Last status change:	  ", &stx.stx_ctime);
	print_time("Last file access:         ", &stx.stx_atime);
	print_time("Last file modification:   ", &stx.stx_mtime);
   
	exit(EXIT_SUCCESS);
       }

