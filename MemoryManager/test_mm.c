#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "MemoryManagerADT.h"

#define MEMTOALLOC 128 * 1024 * 1024
#define MEMFORMM 128
#define MAX_BLOCKS 128

typedef struct MM_rq
{
    void *address;
    uint32_t size;
} mm_rq;

void *memset(void *destiation, int32_t c, uint64_t length)
{
    uint8_t chr = (uint8_t)c;
    char *dst = (char *)destiation;

    while (length--)
    {
        dst[length] = chr;
    }
    return destiation;
}

uint64_t main(uint64_t argc, char *argv[])
{
    void *dir1 = malloc(MEMFORMM);
    void *dir2 = malloc(MEMTOALLOC);

    printf("dir1:%p dir2:%p\n", dir1, dir2);
    MemoryManagerADT mm = createMemoryManager(dir1, dir2);
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if (argc != 2)
    {
        return -1;
    }

    if ((max_memory = satoi(argv[1])) <= 0)
    {
        return -1;
    }
    int j = 0;
    while (1)
    {
        printf("\n-----------------------------------Loop N°%d-----------------------------------\n", j++);
        rq = 0;
        total = 0;

        while (rq < MAX_BLOCKS && total < max_memory)
        {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = allocMemory(mm, mm_rqs[rq].size);
            if (mm_rqs[rq].address)
            {
                total += mm_rqs[rq].size;
                rq++;
            }
            printf("Request:%d Total:%d Address:%p\n", rq, total, mm_rqs[rq - 1].address);
        }

        printf("\n");

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
        {
            if (mm_rqs[i].address)
            {
                memset(mm_rqs[i].address, i, mm_rqs[i].size);
            }
            printf("Set:%d Size:%d Address:%p\n", i, mm_rqs[i].size, mm_rqs[i].address);
        }

        printf("\n");

        // Check
        for (i = 0; i < rq; i++)
        {
            printf("Check:%d Size:%d Address:%p\n", i, mm_rqs[i].size, mm_rqs[i].address);
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
                {
                    printf("test_mm ERROR\n");
                    return -1;
                }
        }

        printf("\n");

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
            {
                printf("Freed:%d Size:%d Address:%p\n", i, mm_rqs[i].size, mm_rqs[i].address);
                freeMemory(mm, mm_rqs[i].address);
            }
    }
}