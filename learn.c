#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
// int v = 5;
// int main()
// {
//     pid_t pid;
//     pid = fork();
//     printf("PID is %d\n", pid);
//     if (pid == 0)
//     {
//         v += 12;
//         printf("Child value is %d\n", v);
//         return 0;
//     }
//     else if (pid > 0)
//     {
//         wait(NULL);
//         printf("Parent value is %d\n", v);
//         return 0;
//     }
// }

/*
 * execvp vs execlp
 * execvp takes an array of arguments (v - vector/arr)
 * execlp takes a list of arguments. (l - list)
 *

// int main()
// {
//     char *args[] = {"ls", "-l", NULL};
//     execvp("ls", args);
//     // execlp("ls", "ls", "-l", NULL);
//     return 0;
// }
*/

// int main(int argc, char *argv[])
// {
//     printf("hello world (pid:%d)\n", (int)getpid());
//     int rc = fork();
//     if (rc < 0)
//     { // fork failed; exit
//         fprintf(stderr, "fork failed\n");
//         exit(1);
//     }
//     else if (rc == 0)
//     { // child (new process)
//         printf("hello, I am child (pid:%d)\n", (int)getpid());
//         char *myargs[3];
//         myargs[0] = strdup("wc"); // program: "wc" (word count)
//         // wc: count lines, words, characters in a file
//         myargs[1] = strdup("p3.c"); // argument: file to count
//         myargs[2] = NULL;           // arr should be ended with NULL
//         execvp(myargs[0], myargs);  // runs word count
//         printf("this shouldn’t print out");
//     }
//     else
//     { // parent goes down this path (main)
//         int wc = wait(NULL);
//         printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
//                rc, wc, (int)getpid());
//     }
//     return 0;
// }

// int main()
// {
//     pid_t pid;
//     pid = fork();
//     if (pid < 0)
//     {
//         fprintf(stderr, "fork failed\n");
//         exit(1);
//     }
//     else if (pid == 0)
//     {
//         execlp("/bin/ls", "ls", NULL);
//         printf("this shouldn't print out\n");
//     }
//     else
//     {
//         wait(NULL);
//         printf("Child process finished executing\n");
//     }
//     return 0;
// }
#define MAXLINE 1024
#define MAXARGS 100
int main(void)
{
    char buf[MAXLINE]; /* from apue.h */
    pid_t pid;
    int status;
    char *token[MAXARGS];
    const char s[2] = " ";
    printf("%% "); /* print prompt (printf requires %% to print %) */
    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */
        // store user's input in buf
        //  tokenize input to extract command and arguments
        int i = 0;
        token[i] = strtok(buf, s);
        while (token[i] != NULL)
        {
            i++;
            token[i] = strtok(NULL, s);
        }
        // token[0] = "./countnames", token[1] = "file1.txt",
        // token[2] = "file2.txt", token[3] = "file3.txt", token[4] = NULL
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
                char *args[3];
                args[0] = token[0];
                args[1] = token[i];
                args[2] = NULL;
                execvp(args[0], args);
                printf("couldn't execute: %s\n", buf);
                exit(127);
            }
        }

        // let's say we want to spawn several child processes (one per input file) to compute the name counts of each file.
        // ex: ./countnames file1.txt file2.txt file3.txt
        // our token array would look like this: token[0] = "./countnames", token[1] = "file1.txt", token[2] = "file2.txt", token[3] = "file3.txt", token[4] = NULL
        // do I need to create each child process in the loop that tokenizes the input?
        // because I want to run ./countnames with each of input file stored in token[1], token[2], token[3] as arguments.
        // should i run a loop through the array or something?

        // wait for all child processes to finish
        pid_t wpid;
        while ((wpid = wait(&status)) > 0)
        {
            printf("Finished executing command: %s\n", buf);
        } // wait for all child processes to finish
        if (wpid == -1 && errno != ECHILD)
        {
            perror("wait error");
            exit(1);
        }
    }
    exit(0);
}
