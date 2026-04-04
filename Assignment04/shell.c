#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "structs.h"
#define MAXLINE 1024
#define MAXARGS 100
#define MAXNAMES 1000
#define MAX_NAMES_PER_FILE 100
#define REGION_SIZE (MAX_NAMES_PER_FILE * (int)sizeof(NameCountData))
/**
 * The function finds the index of a name in array
 */
static int find_name_index(NameCountData entries[], int size, const char *name)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(entries[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}
// the function combines results from child processes and write to shared memory for parent to read
static void summation(ChildResults *summation_region, ChildResults *child_result)
{
    // go through each entry in child_result and add to summation_region
    // example [child 1 result] entries: {name: "Alice", count: 2}, {name: "Bob", count: 3}
    // example [child 2 result] entries: {name: "Alice", count: 1}, {name: "Charlie", count: 4}
    // after summation, summation_region entries: {name: "Alice", count: 3}, {name: "Bob", count: 3}, {name: "Charlie", count: 4}
    for (int i = 0; i < child_result->used; i++)
    {
        NameCountData *child_entry = &child_result->entries[i];
        int index = find_name_index(summation_region->entries, summation_region->used, child_entry->name);
        if (index >= 0)
        {
            summation_region->entries[index].count += child_entry->count;
        }
        else
        {
            if (summation_region->used >= MAXNAMES)
            {
                continue; // skip if summation region is full
            }
            strncpy(summation_region->entries[summation_region->used].name, child_entry->name, MAX_NAME_LEN - 1);
            summation_region->entries[summation_region->used].name[MAX_NAME_LEN - 1] = '\0'; // ensure null termination
            summation_region->entries[summation_region->used].count = child_entry->count;
            summation_region->used++;
        }
    }
}

int main(int argc, char *argv[])
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    char *token[MAXARGS];
    int children = 0;
    const char s[2] = " ";
    printf("%% ");
    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
        {
            buf[strlen(buf) - 1] = '\0'; // remove newline character
        }
        int i = 0;
        token[i] = strtok(buf, s);
        while (token[i] != NULL)
        {
            i++;
            token[i] = strtok(NULL, s);
        }
        if (token[0] == NULL)
        {
            printf("%% ");
            continue; // empty command, prompt again
        }
        // Create the GLOBAL shared memory region
        ChildResults *GLOBAL = mmap(NULL, sizeof(ChildResults), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (GLOBAL == MAP_FAILED)
        {
            perror("mmap");
            printf("%% ");
            continue; // if mmap fails, prompt again
        }
        memset(GLOBAL, 0, sizeof(ChildResults)); // initialize the shared memory region
        children = 0;
        // create child processes to execute countnames for each file
        for (int j = 1; j < i; j++)
        {
            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                continue; // if fork fails, skip this command and prompt again
            }
            else if (pid == 0)
            {
                // child process
                char *args[3];
                args[0] = token[0]; // command name
                args[1] = token[j]; // file name
                args[2] = NULL;     // null-terminate the arguments array

                if (execvp(args[0], args) == -1)
                {
                    perror("execvp");
                }
                exit(1); // if execlp fails, exit child process
            }
            else
            {
                // parent process
                children++;
            }
        }
        // wait for all child processes to finish
        while (children > 0)
        {
            if ((pid = waitpid(-1, &status, 0)) < 0)
            {
                perror("waitpid");
            }
            else
            {
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                {
                    //  read child's result from PID.out
                    //  accumulate results to GLOBAL shared memory region using summation function
                    char outfile[64];
                    snprintf(outfile, sizeof(outfile), "%d.out", pid);
                    int fd = open(outfile, O_RDONLY);
                    if (fd < 0)
                    {
                        perror("open child output file");
                    }
                    else
                    {
                        ChildResults *child = mmap(NULL, sizeof(ChildResults), PROT_READ, MAP_SHARED, fd, 0);
                        if (child == MAP_FAILED)
                        {
                            perror("mmap child output file");
                        }
                        else
                        {
                            summation(GLOBAL, child); // combine child result to GLOBAL shared memory region
                            munmap(child, sizeof(ChildResults));
                        }
                        close(fd);
                    }
                }
                else
                {
                    fprintf(stderr, "Child process %d fails\n", pid);
                }
            }
            children--;
            // after all child processes finish, read their results from GLOBAL shared memory region and combine them
        }
        for (int i = 0; i < GLOBAL->used; i++)
        {
            printf("%s: %d\n", GLOBAL->entries[i].name, GLOBAL->entries[i].count);
        }
        munmap(GLOBAL, sizeof(ChildResults)); // unmap the shared memory region

        return 0;
    }
}