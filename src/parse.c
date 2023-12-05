#include <stdio.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "parse.h"

int remove_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *remove)
{
    int employee_index = -1;

    for (int i = 0; i < dbhdr->count; i++)
    {
        if (strcmp(remove, employees[i].name) == 0)
        {
            printf("index of employee %d", i);
            employee_index = i;
            break;
        }
    }

    if (employee_index != -1)
    {
        for (int i = employee_index; i < dbhdr->count - 1; i++)
        {
            employees[i] = employees[i + 1];
        }

        return STATUS_SUCCESS;
    }

    return STATUS_ERROR;
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees)
{
    for (int i = 0; i < dbhdr->count; i++)
    {
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n", employees[i].hours);
    }
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring)
{
    printf("%s\n", addstring);

    char *name = strtok(addstring, ",");

    char *addr = strtok(NULL, ",");

    char *hours = strtok(NULL, ",");

    printf("%s %s %s\n", name, addr, hours);

    strncpy(employees[dbhdr->count - 1].name, name, sizeof(employees[dbhdr->count - 1].name));
    strncpy(employees[dbhdr->count - 1].address, addr, sizeof(employees[dbhdr->count - 1].address));

    employees[dbhdr->count - 1].hours = atoi(hours);

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut)
{
    if (fd < 0)
    {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL)
    {
        printf("Malloc failed\n");
        return STATUS_ERROR;
    }

    read(fd, employees, count * sizeof(struct employee_t));

    for (int i = 0; i < count; i++)
    {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees)
{
    if (fd < 0)
    {
        printf("Got a bad FD from the user\n");
    }

    int realcount = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr->count = htons(dbhdr->count);
    dbhdr->version = htons(dbhdr->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhdr, sizeof(struct dbheader_t));

    for (int i = 0; i < realcount; i++)
    {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    ftruncate(fd, sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
}

int validate_db_header(int fd, struct dbheader_t **headerOut)
{
    if (fd < 0)
    {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL)
    {
        printf("Malloc failed create a db header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t))
    {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->magic != HEADER_MAGIC)
    {
        printf("Improper header magic\n");
        free(header);
        return -1;
    }

    if (header->version != 1)
    {
        printf("Improper header version\n");
        free(header);
        return -1;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size)
    {
        printf("Corrupted database\n");
        free(header);
        return -1;
    }

    *headerOut = header;

    return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut)
{
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL)
    {
        printf("Malloc failed to create db header\n");
        return STATUS_ERROR;
    }

    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    return STATUS_SUCCESS;
}
