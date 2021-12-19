#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>


int main(void) {
    // UID and user name
    

    // PID, PPID
    // Each process has a unique non-negative integer identifier that is assigned when the process is created using fork -- PID
    // A process's parent process ID identifies the process thet created this process using fork(2) -- PPID
    pid_t process_id = getpid();//These functions are always successful too.
    pid_t parent_pid = getppid();
    printf("Process ID: %d\nParent's process ID: %d\n", process_id, parent_pid);
    //Process Group ID and Session ID
    pid_t process_group_id = getpgrp();
    pid_t session_id = getsid(process_id);
    printf("Process Group ID: %d\nSession ID: %d\n", process_group_id, session_id);

    // get user name
    uid_t uid = getuid();
    struct passwd* pass_info;
    const char* user_name;
    pass_info = getpwuid(uid);
    if(pass_info == NULL) {
        user_name = "?";
    } else {
        user_name = pass_info->pw_name;
    }
    printf("User ID = %d, %s\n", uid, user_name);

    
    //User and Group Identifiers
    //Real user ID and real group ID. These ID's determine who owns the process

    gid_t gid = getgid();
    const char* grp_name;
    struct group* grp_info = getgrgid(gid);
    if(grp_info == NULL) {
        grp_name = "?";
    } else {
        grp_name = grp_info->gr_name;
    }
    printf("Group ID = %d, %s\n", gid, grp_name);

    //Effective user ID and effective group ID
    
    printf("Effective user ID = %d\n", geteuid());
    printf("Effective group ID = %d\n", getegid());
 

    // all groups
    int j, ngroups;
    gid_t *groups;
    struct passwd *pw;
    struct group *gr;
    //если размер size равен нулю, и список равен NULL, то просто возвращается
    //общее количество идентификаторов дополнительных групп для текущего процесса
    ngroups = getgroups(0, NULL);
    groups = (gid_t *)calloc((size_t)ngroups, sizeof (gid_t));
    if (groups == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    //returns a pointer to a structure with passwords
    pw = getpwnam(user_name);
    if (pw == NULL) {
        perror("getpwnam");
        exit(EXIT_SUCCESS);
    }
    // get list of groups to which a user belongs
    if (getgroups(ngroups, groups) == -1) {
        fprintf(stderr, "getgrouplist() вернула -1; ngroups = %d\n",
                ngroups);
        exit(EXIT_FAILURE);
    }
    //print list of groups with names
    for (j = 0; j < ngroups; j++) {
        printf("Group (name): %d", groups[j]);
        gr = getgrgid(groups[j]);
        if (gr != NULL) {
            printf(" (%s)", gr->gr_name);
        }
        printf("\n");
    }

    //set file mode creation mask
    mode_t u_mask = umask(ALLPERMS);
    printf("Umask: %o\n", u_mask);
    //umask sets the calling process's file mode creation mask to mask&0777
    //(only the file permissions bits of mask are used)
    //returns the previous value of the mask
    //thats why we need to return the previous value 
    u_mask = umask(u_mask);

    //get sheduling priority
    int pr = getpriority(PRIO_PROCESS, 0);
    if(pr == -1) {
        perror("Failure getpriority");
        exit(EXIT_FAILURE);
    }
    printf("Sheduling priority: %d\n", pr);
    free(groups);
    return 0;
}
