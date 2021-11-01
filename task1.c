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

#define statx foo;
#define statx_timestamp foo_timestamp;
struct statx;
struct statx_timestamp
#undef statx
#undef statx_timestamp

static void print_time(const char *field, struct statx_timestamp *ts)
{
	time_t tim;
	struct tm* ptm;
	char time_string[128];
	long milliseconds;

	tim = ts->tv_sec;
	ptm = localtime(&tim);
	strftime(time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
	milliseconds = ts->tv_nsec /1000000;
	printf("%s.%03ld\n", time_string, milliseconds);

	/*struct tm tm;
	time_t tim;
	char buffer[100];
	int len;

	tim = ts->tv_sec;
	if(localtime_r(&tim, &tm)) {
		perror("localtime_r");
		exit(1);
	}
	len = strftime(buffer, 100, "%F %T", &tm);
	if (len == 0){
		perror("strftime");
		exit(1);
	}
	printf("%s", field);
	fwrite(buffer, 1, len, stdout);
	printf(".%09u", ts->tv_nsec);
	len = strftime(buffer, 100, "%z", &tm);
	if (len == 0) {
		perror("strftime2");
		exit(1);
	}
	fwrite(buffer,*/ 
}

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


char * rights (int mode)
{
	char * mode_rights = malloc (10*sizeof(char));
	int i = 8;
	for (i = 8; i >= 0; i--)
	       mode_rights[8-i] = (mode & (1 << i) ? "xwr"[i%3]: '-');
	mode_rights[9] = '\0';
	return mode_rights;
}


int main(int argc, char *argv[])
{
	struct stat sb;
	struct statx stx;
	unsigned int mask = STATX_BASIC_STATS | STATX_BTIME;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (lstat(argv[1], &sb) == -1) {
		perror("lstat");
		exit(EXIT_FAILURE);
	}
	printf("File name %s\n", argv[1]);
	printf("ID of containing device: [%lxh, %ldd]\n", (long) (sb.st_dev), (long) (sb.st_dev));
	fputs("File type:		");
	printf("%s", filetype(sb.st_mode));
	printf("I-node number: %ld\n", (long) sb.st_ino);
	printf("Mode_rights: (%04o/%s)\n", sb.st_mode & ALLPERMS, rights(sb.st_mode & ALLPERMS));
	printf("Link count: %ld\n", (long) sb.st_nlink);
	printf("Ownership UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
	printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
	printf("File size: %lld bytes\n", (long long) sb.st_size);
	printf("Blocks allocated: %lld\n", (long long) sb.st_blocks);
	printf("Last status change:	  %s", ctime(&sb.st_ctime));
	printf("Last file access:         %s", ctime(&sb.st_atime));
	printf("Last file modification:   %s", ctime(&sb.st_mtime));

	memset(&stx, 0xbf, sizeof(stx));

	if (stx.stx_mask & STATX_CTIME)
		print_time("Modification: ", &(stx.stx_mtime));

           exit(EXIT_SUCCESS);
       }

