#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "structs.h"

#define BUF_SIZE 256
/**
 * This function performs a linear search to find the index of a name in the names array.
 * Assumption: the names array is not sorted and contains unique names.
 * Input parameters:
 *      - names: a 2D array of strings containing the names
 *      - size: the number of names currently stored in the names array
 *      - name: the name to search for
 * Returns: the index of the name if found, or -1 if not found
 **/
static int find_name_index(char names[MAX_NAMES][MAX_NAME_LEN], int size, const char *name)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(names[i], name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    FILE *fptr = NULL;

    // load file
    if (argc == 2)
    {
        fptr = fopen(argv[1], "r");
        if (fptr == NULL)
        {
            // exit with code status 1 if can not open file
            fprintf(stderr, "error: cannot open file %s\n", argv[1]);
            return 1;
        }
    }
    else if (argc == 1)
    {
        fptr = stdin;
    }
    else
    {
        return 0; // no file or other parameters -> exit 0
    }

    char names[MAX_NAMES][MAX_NAME_LEN];
    int counts[MAX_NAMES];
    int size = 0;

    memset(counts, 0, sizeof(counts));
    memset(names, 0, sizeof(names));

    char buf[BUF_SIZE];
    int line_count = 0;
    // print err in .err file if line is empty
    int errfd = -1;
    char errfile[64];
    snprintf(errfile, sizeof(errfile), "%d.err", getpid());

    errfd = open(errfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (errfd < 0)
    {
        perror("open .err file");
        return 1;
    }

    while (fgets(buf, sizeof(buf), fptr) != NULL)
    {
        line_count++;
        // Since each name is seperated by newline, we need to remove
        // the newline character at the end of the name.
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
        {
            buf[len - 1] = '\0';
        }
        // trim trailing whitespace (spaces, tabs, \r, etc.)
        size_t end = strlen(buf);
        while (end > 0 && (buf[end - 1] == ' ' || buf[end - 1] == '\t' || buf[end - 1] == '\r'))
        {
            buf[end - 1] = '\0';
            end--;
        }

        // skip leading whitespaces
        char *p = buf;
        while (*p == ' ' || *p == '\t' || *p == '\r')
            p++;

        // now p points to trimmed content
        if (*p == '\0')
        {
            dprintf(errfd, "Warning - file %s line %d is empty.\n",
                    argc == 2 ? argv[1] : "stdin", line_count);
            continue;
        }

        char name[MAX_NAME_LEN] = {0};
        strncpy(name, p, MAX_NAME_LEN - 1);
        name[MAX_NAME_LEN - 1] = '\0';

        int index = find_name_index(names, size, name);
        // if name existed, increment count;
        // otherwise, add name to names array and set count to 1
        if (index >= 0)
        {
            counts[index]++;
        }
        else
        {
            if (size >= MAX_NAMES)
            {
                continue;
            }
            strncpy(names[size], name, MAX_NAME_LEN - 1);
            names[size][MAX_NAME_LEN - 1] = '\0';
            counts[size] = 1;
            size++;
        }
    }
    if (size == 0)
    {
        return 0; // empty file -> exit with status code 0
    }
    if (fptr != NULL && fptr != stdin)
    {
        fclose(fptr);
    }

    /* open .out file AFTER we know there is output */
    int outfd = -1;
    char outfile[64];
    snprintf(outfile, sizeof(outfile), "%d.out", getpid());
    outfd = open(outfile, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0)
    {
        perror("open .out file");
        return 1;
    }

    size_t filesize = sizeof(ChildResults); // size of shared memory region
    if (ftruncate(outfd, filesize) == -1)
    {
        perror("ftruncate");
        close(outfd);
        return 1;
    } // set the file size to the size of ChildResults struct, so that we can mmap it for shared memory
    ChildResults *map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, outfd, 0);
    if (map == MAP_FAILED)
    {
        perror("mmap");
        close(outfd);
        return 1;
    }

    map->used = size; // mark this slot as used
    // let say child_index is 0, then we write the name count data to the first
    //  slot of shared memory
    //  shared[0].entries = array of NameCountData struct, each struct contains name and count
    //  shared[0].entries[0] = {name: "Alice", count: 3} // first element of the array
    //  and so on for other names and counts
    //  general: shared[child_index].entries[i] = {name: names[i], count: counts[i]} for i in 0 to size-1
    for (int i = 0; i < size; i++)
    {
        strncpy(map->entries[i].name, names[i], sizeof(map->entries[i].name) - 1);
        map->entries[i].name[sizeof(map->entries[i].name) - 1] = '\0'; // ensure null termination
        map->entries[i].count = counts[i];

        // write the struct data shared memory with parent using mmap
        // dprintf(outfd, "%s: %d\n", map->entries[i].name, map->entries[i].count);
    }

    if (msync(map, filesize, MS_SYNC) == -1)
    {
        perror("msync");
    }
    if (munmap(map, filesize) == -1)
    {
        perror("munmap");
    }
    /* ---- human-readable .txt ---- */
    char txtfile[64];
    snprintf(txtfile, sizeof(txtfile), "%d.txt", getpid());
    FILE *txtfd = fopen(txtfile, "w");
    if (txtfd == NULL)
    {
        perror("open .txt file");
        return 1;
    }
    for (int i = 0; i < size; i++)
    {
        fprintf(txtfd, "%s: %d\n", names[i], counts[i]);
    }

    fclose(txtfd);
    close(outfd);
    close(errfd);
    return 0;
}