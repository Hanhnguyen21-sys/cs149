#ifndef STRUCTS_H
#define STRUCTS_H

#include <stddef.h>

// struct is used to return data from the child:
typedef struct
{
    char name[30];
    int count;
} NameCountData;

typedef enum
{
    TYPE_NAMECOUNT,
    TYPE_B // there is a possibility to extend with more types in the future
} MessageType;

typedef struct
{
    MessageType type;
    size_t size; // size of the payload that follows
} MessageHeader;

#endif