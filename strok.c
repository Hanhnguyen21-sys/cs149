
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main(void)
{
    pid_t pid;
    int status;
    char buf[1][5];
    char *buf2[6];
    buf[0][0] = '.';
    buf[0][1] = '.';
    buf[0][2] = 0;
    buf2[0] = "ls";
    buf2[1] = "-latr";
    buf2[2] = "/etc/passwd";
    buf2[3] = (char *)&buf[0][0];
    buf2[4] = (char *)0;
    buf2[5] = (char *)0;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid == 0)
    { /* child */
        execvp(buf2[0], buf2);
        printf("couldn't execute: %s\n", buf2[0]);
        exit(127);
    }
    /* parent */
    if ((pid = waitpid(pid, &status, 0)) < 0)
    {
        perror("waitpid error");
        exit(1);
    }
    return 0;
}
