#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TABLE_SIZE 100
typedef struct Node
{
    char *name;
    int count;
    struct Node *next; // use for chaining in case of hash collision
} Node;
// implement a hash table to store the names and counts
// Ex: "Alice" -> 3
// "John"  -> 5
// "Alex"  -> 2

// symbol hash function
unsigned int hash(const char *str)
{
    unsigned int hash = 5381; // a common starting value for hash functions
    int c;
    while ((c = *str++)) // iterate through each character in the string
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TABLE_SIZE; // return the index in the hash table
}
void insert_or_increment(Node *table[], const char *name)
{
    unsigned int index = hash(name); // calculate index using hash function
    Node *current = table[index];
    while (current != NULL)
    {
        // increment count if name already exists
        if (strcmp(current->name, name) == 0)
        {
            current->count++;
            return;
        }
        // move to the next node in the chain
        current = current->next;
    }
    Node *new_node = (Node *)malloc(sizeof(Node)); // create a new node
    new_node->name = strdup(name);                 // duplicate the name string
    new_node->count = 1;                           // initialize count to 1
    new_node->next = table[index];                 // insert at the beginning of the chain
    table[index] = new_node;                       // update the head of the chain in the hash table
}

void print_table(Node *table[])
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Node *current = table[i];
        while (current != NULL)
        {
            printf("%s: %d\n", current->name, current->count); // print name and count
            current = current->next;                           // move to the next node in the chain
        }
    }
}
void free_table(Node *table[])
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        Node *current = table[i];
        while (current != NULL)
        {
            Node *temp = current;
            current = current->next; // move to the next node before freeing
            free(temp->name);        // free the duplicated name string
            free(temp);              // free the node itself
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    Node *table[TABLE_SIZE] = {0};    // initialize hash table with NULL
    FILE *file = fopen(argv[1], "r"); // open the file containing names
    if (file == NULL)
    {
        perror("Failed to open file");
        return 1;
    }
    // read line by line
    char *line = (char *)malloc(256); // buffer to hold each line

    while (fgets(line, 256, file))
    {
        // remove newline
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0')
        {
            // print out waring message for empty line
            fprintf(stderr, "Warning: Skipping empty line in input file.\n");
            continue;
        }
        // skip empty lines
        insert_or_increment(table, line); // insert or increment the count for the name
    }
    free(line);         // free the line buffer
    fclose(file);       // close the file
    print_table(table); // should print Alice: 2 and Bob: 1
    free_table(table);  // free allocated memory
    return 0;
}