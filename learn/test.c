#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char name[30];
    int count;
} NameCountData;

typedef enum
{
    TYPE_NAMECOUNT,
    TYPE_B
} MessageType;

typedef struct
{
    MessageType type;
    size_t size;
} MessageHeader;

void write_struct_namecount(int fd, NameCountData *data)
{
    MessageHeader header;
    header.type = TYPE_NAMECOUNT;
    header.size = sizeof(NameCountData);

    write(fd, &header, sizeof(header));
    write(fd, data, sizeof(NameCountData));
}

int main()
{
    int fd[2];
    pipe(fd);

    NameCountData data1 = {"alice", 3};
    NameCountData data2 = {"bob", 2};
    NameCountData data3 = {"mary", 5};

    // simulate child writing
    write_struct_namecount(fd[1], &data1);
    write_struct_namecount(fd[1], &data2);
    write_struct_namecount(fd[1], &data3);
    close(fd[1]);

    // simulate parent reading
    MessageHeader header;
    NameCountData received;

    while (read(fd[0], &header, sizeof(header)) > 0)
    {

        switch (header.type)
        {

        case TYPE_NAMECOUNT:
        {
            NameCountData received;

            read(fd[0], &received, header.size);

            printf("Received: %s %d\n", received.name, received.count);

            break;
        }

        default:
            fprintf(stderr, "Unknown message type\n");
        }
    }
    while (read(fd[0], &header, sizeof(header)) > 0)
    {
        if (header.type == TYPE_NAMECOUNT)
        {
            read(fd[0], &received, sizeof(received));
            printf("Received: name=%s count=%d\n", received.name, received.count);
        }
        close(fd[0]);
    }
    return 0;
}