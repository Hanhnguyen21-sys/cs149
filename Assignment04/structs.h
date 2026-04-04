#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_NAMES 100
#define MAX_NAME_LEN 31
#include <stddef.h>
typedef struct
{
    char name[MAX_NAME_LEN];
    int count;
} NameCountData;
// expand struct for using mmap
typedef struct
{
    int used;                         // flag to indicate if this slot is used
    NameCountData entries[MAX_NAMES]; // the actual name count data
} ChildResults;

typedef enum
{
    TYPE_NAMECOUNT,
    TYPE_B // there is a possibility to extend with more types in the future
} MessageType;

typedef struct
{
    MessageType type;
    size_t size; // Size of the following payload
} MessageHeader;

#endif // STRUCTS_H