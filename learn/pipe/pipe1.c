#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// parent -> child communication using pipe
/**
 * Parent sends a message to child through pipe,
 * child reads the message and prints it to stdout.
 */
int main()
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        // child process
        // child reads from pipe -> close write end
        close(fd[1]);
        char buffer[100];
        read(fd[0], buffer, sizeof(buffer));
        printf("Child received message: %s\n", buffer);
        close(fd[0]);
    }
    else
    {
        // parent process
        close(fd[0]); // close read end
        char message[] = "Hello from parent!";
        write(fd[1], message, strlen(message) + 1);
        printf("Parent sent message: %s\n", message);
        close(fd[1]);
        wait(NULL); // wait for child to finish
    }
    return 0;
}
