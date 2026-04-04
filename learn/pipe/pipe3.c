#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#define MAXLINE 100
int main()
{
    int p_to_c[2];
    int c_to_p[2];
    pid_t pid;
    char message[MAXLINE];
    char result[MAXLINE];
    if (pipe(p_to_c) == -1 || pipe(c_to_p) == -1)
    {
        perror("pipe");
        exit(1);
    }
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        // in child process
        //  read message from parent and convert to uppercase -> read pipe1
        //  then send back to parent via pipe2 -> write pipe2
        close(p_to_c[1]); // close write end of pipe1
        close(c_to_p[0]); // close read end of pipe2
        // read from parent
        read(p_to_c[0], message, sizeof(message));
        // convert to uppercase
        for (int i = 0; message[i]; i++)
        {
            if (message[i] >= 'a' && message[i] <= 'z')
            {
                message[i] = message[i] - 'a' + 'A';
            }
        }
        // send back to parent
        write(c_to_p[1], message, strlen(message) + 1);
        close(p_to_c[0]);
        close(c_to_p[1]);
        exit(0);
    }
    else
    {
        // in parent process
        //  send message to child via pipe1 -> write pipe1
        //  then read back from child via pipe2 -> read pipe2
        close(p_to_c[0]); // close read end of pipe1
        close(c_to_p[1]); // close write end of pipe2
        printf("Enter a message: ");
        if (fgets(message, sizeof(message), stdin) == NULL)
        {
            perror("fgets");
            exit(1);
        }
        // remove newline character from message
        message[strcspn(message, "\n")] = '\0';
        // send to child
        write(p_to_c[1], message, strlen(message) + 1);
        // read result from child
        read(c_to_p[0], result, sizeof(result));
        printf("Result from child: %s\n", result);
        wait(NULL); // wait for child to finish
        close(p_to_c[1]);
        close(c_to_p[0]);
    }
    return 0;
}