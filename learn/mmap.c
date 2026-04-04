#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 100
#define MAX_NAMES 100
#define MAX_NAME_LEN 31
#define NUM_CHILDREN 4
typedef struct
{
    char name[MAX_NAME_LEN];
    int count;
} NameCountData;
typedef struct
{
    int used;                         // flag to indicate if this slot is used
    NameCountData entries[MAX_NAMES]; // the actual name count data
} ChildResults;

int main(void)
{

    // parent creates shared memory using mmap
    ChildResults *shared = mmap(NULL, sizeof(ChildResults) * 10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    // shared will be an array of 10 ChildResults struct, each struct contains a flag to indicate if this slot is used and an array of NameCountData struct to store the name count data from each child process
    //  shared[0] -> child 0
    //  shared[1] -> child 1
    //  and so on for other child processes

    // initialize slot with 0
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        shared[i].used = 0;
    }

    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        }
        else if (pid == 0)
        {
            shared[i].used = 2;
            // child process: write name count data to shared memory
            snprintf(shared[i].entries[0].name, MAX_NAME_LEN, "Child%d_Name1", i);
            shared[i].entries[0].count = i + 1;
            snprintf(shared[i].entries[1].name, MAX_NAME_LEN, "Child%d_Name2", i);
            shared[i].entries[1].count = (i + 1) * 10;
            munmap(shared, sizeof(ChildResults) * 10); // unmap shared memory in child process
            exit(0);
        }
    }
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        // parent waits for all child processes to finish
        wait(NULL); // wait for all child processes to finish
    }

    // parent print out result
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        if (shared[i].used > 0)
        {
            printf("Child %d:\n", i);
            for (int j = 0; j < MAX_NAMES && shared[i].entries[j].count > 0; j++)
            {
                printf("  %s: %d\n", shared[i].entries[j].name, shared[i].entries[j].count);
            }
        }
    }
    munmap(shared, sizeof(ChildResults) * 10); // unmap shared memory in parent process
    return 0;
}
