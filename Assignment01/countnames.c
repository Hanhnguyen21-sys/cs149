#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct entry
{
    char name[100];
    int count;
    struct entry *next;
} entry;
/**
 * Create a hash table to store a pair value (name) - key (its occurrence)
 */
#define TABLE_SIZE 101
static entry *table[TABLE_SIZE] = {0}; // table is an array pointer

// create bucket index by converting a string
static unsigned hash(const char *s)
{
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*s++))
    {
        h = ((h << 5) + h) + (unsigned)c; // h*33 + c
    }
    return (unsigned)(h % TABLE_SIZE);
}
// find name in the table, return the entry if found, otherwise return NULL
static entry *table_of_fre(const char *name)
{
    unsigned idx = hash(name);
    entry *cur = table[idx];
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}
// add new entry to table
static void add_to_table(entry *e)
{
    unsigned idx = hash(e->name);
    e->next = table[idx];
    table[idx] = e;
}
// print out hash table
static void print_table(void)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        entry *cur = table[i];
        while (cur != NULL)
        {
            printf("%s: %d\n", cur->name, cur->count);
            cur = cur->next;
        }
    }
}
// argv[0] - program name (./countnames), argv[1] - file name (names_long.txt)
int main(int argc, char *argv[])
{
    FILE *fp = NULL;

    if (argc == 1)
    {
        // read from stdin
        fp = stdin;
    }
    else if (argc == 2)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "error: cannot open file\n");
            return 1;
        }
    }
    else
    {
        // spec: if no file or other parameters -> exit 0
        // (they said no parameters OR empty file should exit 0;
        // for extra args, safest is just exit 0 unless your instructor says otherwise)
        return 0;
    }

    char line[256];
    int cnt = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        cnt++;

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if (strlen(line) < 1)
        {
            fprintf(stderr, "Warning - Line %d is empty.\n", cnt);
            continue;
        }

        entry *e = table_of_fre(line);
        if (e)
        {
            e->count++;
        }
        else
        {
            e = malloc(sizeof(entry));
            if (!e)
                return 1;
            strncpy(e->name, line, sizeof(e->name) - 1);
            e->name[sizeof(e->name) - 1] = '\0';
            e->count = 1;
            e->next = NULL;
            add_to_table(e);
        }
    }

    if (fp != stdin)
        fclose(fp);

    // print format must match expected
    print_table();

    return 0;
}
