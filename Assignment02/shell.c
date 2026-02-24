#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXLINE 1024
#define MAXARGS 100
int main(void)
{
    pid_t pid;
    int status;
    char buf[MAXLINE];
    char *token[MAXARGS];
    const char s[2] = " ";
    int children = 0; // number of children processes
    printf("%% ");
    while (fgets(buf, MAXLINE, stdin) != NULL)
    {

        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */

        // tokenize input to extract command and arguments
        int i = 0;
        token[i] = strtok(buf, s);
        while (token[i] != NULL)
        {
            i++;
            token[i] = strtok(NULL, s);
        }

        // reprompt if no command is entered
        if (token[0] == NULL)
        {
            printf("%% ");
            continue;
        }
        // Fork a child for each argument
        for (int i = 1; token[i] != NULL; i++)
        {
            pid = fork();
            if (pid < 0)
            {
                perror("fork error");
                exit(1);
            }

            else if (pid == 0)
            {
                char outfile[64], errfile[64];
                snprintf(outfile, sizeof(outfile), "%d.out", getpid());
                snprintf(errfile, sizeof(errfile), "%d.err", getpid());

                /* Redirect stdout → <pid>.out */
                int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0)
                {
                    perror("open .out");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);

                /* Redirect stderr → <pid>.err */
                int fd_err = open(errfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_err < 0)
                {
                    perror("open .err");
                    exit(1);
                }
                dup2(fd_err, STDERR_FILENO);
                close(fd_err);

                char *args[3];
                args[0] = token[0];
                args[1] = token[i];
                args[2] = NULL;
                execvp(args[0], args);
                // for each file, we want ./countnames run on each .txt file
                // it will count occurence of names on each file and write output to a file named <pid>.out
                // there are empty lines in input files, hence we write message "Line %d is empty" to <pid>.err file
                printf("couldn't execute: %s\n", args[0]);
                exit(127);
            }
            else
            {
                children++;
            }
        }
        // wait for all children processes to be complete
        // waitpid() returns the PID of finished child process
        while (children > 0)
        {
            if ((pid = waitpid(-1, &status, 0)) < 0)
                perror("waitpid error");
            else
                children--; // decrement children count for each finished child process
        }

        printf("%% ");
    }
    exit(0);
}