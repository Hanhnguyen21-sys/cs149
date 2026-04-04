#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
// one pipe not enough for two way communication, need two pipes
// parent and child can both read and write
int main()
{
    int p_to_c[2]; // parent to child
    int c_to_p[2]; // child to parent
    if (pipe(p_to_c) == -1 || pipe(c_to_p) == -1)
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
        // send message to parent
        char *msg = "Hello there! How are you?";
        close(c_to_p[0]); // close read end of child to parent pipe
        write(c_to_p[1], msg, strlen(msg) + 1);
        // read message from parent
        char buffer[100];
        close(p_to_c[1]); // close write end of parent to child pipe
        read(p_to_c[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(c_to_p[1]); // close write end of child to parent pipe
        close(p_to_c[0]); // close read end of parent to child pipe
    }
    else
    {
        // parent process
        // read message from child
        char buffer[100];
        close(c_to_p[1]); // close write end of child to parent pipe
        read(c_to_p[0], buffer, sizeof(buffer));
        printf("Parent received: %s\n", buffer);
        // send message to child
        char *msg = "I'm not fine :(";
        close(p_to_c[0]); // close read end of parent to child pipe
        write(p_to_c[1], msg, strlen(msg) + 1);
        close(c_to_p[0]); // close read end of child to parent pipe
        close(p_to_c[1]); // close write end of parent to child pipe
        wait(NULL);       // wait for child to finish
    }
    return 0;
}