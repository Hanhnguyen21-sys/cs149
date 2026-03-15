#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#define MAX_NAMES 100
#define MAX_NAME_LEN 31 // 30 chars + '\0'
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
/**
 * This function writes the name count data to the specified file descriptor
 * in a structured format.
 * Input parameters:
 *     - fd: the file descriptor to write to
 *     - data: a pointer to the NameCountData structure
 *       containing the name and count information
 *
 */
void write_struct_namecount(int fd, NameCountData *data)
{
    MessageHeader header;
    header.type = TYPE_NAMECOUNT;
    header.size = sizeof(NameCountData);

    write(fd, &header, sizeof(header));     // Write the header
    write(fd, data, sizeof(NameCountData)); // Write the payload (name counts)
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

    // read from file
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
            fprintf(stderr, "Warning - file %s line %d is empty.\n",
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

    // for (int i = 0; i < size; i++)
    //     printf("%s: %d\n", names[i], counts[i]);
    for (int i = 0; i < size; i++)
    {
        NameCountData data;
        strncpy(data.name, names[i], sizeof(data.name) - 1);
        data.name[sizeof(data.name) - 1] = '\0';
        data.count = counts[i];
        write_struct_namecount(STDOUT_FILENO, &data);
    }
    return 0;
}