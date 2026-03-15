#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "structs.h"
#define MAXLINE 1024
#define MAXARGS 100
#define MAXNAMES 1000
/**
 * this function ensures that all bytes are read from
 * sthe file descriptor until the specified count is reached.
 */
ssize_t read_all(int fd, void *buf, size_t count)
{
    size_t total = 0;
    char *p = buf;

    while (total < count)
    {
        ssize_t n = read(fd, p + total, count - total);
        if (n <= 0)
            return n;
        total += n;
    }
    return total;
}
/**
 * Calculate total name counts
 */
void add_to_totals(NameCountData totals[], int *size, NameCountData data)
{
    for (int i = 0; i < *size; i++)
    {
        // if already exists, update count
        if (strcmp(totals[i].name, data.name) == 0)
        {
            totals[i].count += data.count;
            return;
        }
    }
    // if name not found, add new entry

    if (*size < MAXNAMES)
    {
        strncpy(totals[*size].name, data.name, sizeof(totals[*size].name) - 1);
        totals[*size].name[sizeof(totals[*size].name) - 1] = '\0';
        totals[*size].count = data.count;
        (*size)++;
    }
}
/**
 * This function reads data from the specified file descriptor,
 *  processes it based on the message type, and prints the name count
 * information if the message type is TYPE_NAMECOUNT.
 * Input parameters:
 *      - fd: the file descriptor to read from
 *      - totals: an array to accumulate name count data
 *      - size: a pointer to the current size of the totals array
 */
void read_from_pipe(int fd, NameCountData totals[], int *size)
{
    MessageHeader header;
    // Loop on read to ensure all header bytes arrive

    while (1)
    {
        ssize_t n = read_all(fd, &header, sizeof(header));
        if (n == 0)
        {
            break; // EOF reached, exit loop
        }
        else if (n < 0)
        {
            perror("read error");
            break;
        }
        switch (header.type)
        {
        case TYPE_NAMECOUNT:
        {
            NameCountData dataNC;

            read_all(fd, &dataNC, sizeof(dataNC));
            // Process NameCountData data
            // accumlate counts for the same name if needed,
            // or store in a data structure
            add_to_totals(totals, size, dataNC);
            // printf("Received NameCountData: name=%s, count=%d\n", dataNC.name, dataNC.count);
            break;
        }
        case TYPE_B:
        {
            // This case is for a possible extension in future to process other Struct data
            break;
        }
        default:
            // Handle unknown type error
            fprintf(stderr, "Unknown message type received: %d\n", header.type);
        }
    }
}

int main(void)
{

    char buf[MAXLINE];
    pid_t pid;
    int status;
    char *token[MAXARGS];
    int children = 0; // number of children processes

    const char s[2] = " ";
    printf("%% "); /* print prompt (printf requires %% to print %) */

    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        int size = 0;                   // size of totals array
        NameCountData totals[MAXNAMES]; // array to accumulate name count data from children
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

        for (int i = 1; token[i] != NULL; i++)
        {
            // create pipe for parent-child communication
            int pipefd[2]; // pipefd[0] for reading, pipefd[1] for writing

            if (pipe(pipefd) == -1)
            {
                perror("pipe error");
                exit(1);
            }
            // fork a child for each argument
            pid = fork();
            if (pid < 0)
            {
                perror("fork error");
                exit(1);
            }
            else if (pid == 0)
            {
                // each child wrties to the pipe Header and NameCountData
                // for parent to read
                close(pipefd[0]); // close read end in child
                // redirect stdout to pipe
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]); // close original write end after dup2
                // char outfile[64];
                char errfile[64];
                // snprintf(outfile, sizeof(outfile), "%d.out", getpid());
                snprintf(errfile, sizeof(errfile), "%d.err", getpid());
                // redirect stdout -> <pid>.out
                // int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                // if (fd_out < 0)
                // {
                //     perror("open .out");
                //     exit(1);
                // }
                // dup2(fd_out, STDOUT_FILENO);
                // close(fd_out);

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
                perror("execvp failed");
                exit(1);
            }
            else
            {
                // parent process: close write end and keep read end to read data from child

                close(pipefd[1]);                         // close write end in parent
                read_from_pipe(pipefd[0], totals, &size); // read data from child and process
                close(pipefd[0]);                         // close read end after reading
                children++;
            }
        }

        while (children > 0)
        {
            if ((pid = waitpid(-1, &status, 0)) < 0)
            {
                perror("Waitpid error");
            }
            else
            {
                children--; // decrement children count for each finished child process
            }
        }
        printf("Result:\n");
        int acc_count = 0; // count of unique names
        for (int i = 0; i < size; i++)
        {
            printf("%s: %d\n", totals[i].name, totals[i].count);
            acc_count++;
        }
        printf("Total name counted: %d\n", acc_count);
        printf("%% ");
    }
    exit(0);
}
