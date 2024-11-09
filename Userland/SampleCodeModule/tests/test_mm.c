#include "test_util.h"
#include "libc.h"

#define MAX_BLOCKS 128

typedef struct MM_rq
{
    void *address;
    uint32_t size;
} mm_rq;

void setMem(void *ptr, uint8_t value, uint32_t size)
{
    uint8_t *p = (uint8_t *)ptr;
    uint32_t i;
    for (i = 0; i < size; i++, p++)
    {
        *p = value;
    }
}

int test_mm(int argc, char *argv[])
{

    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if (argc != 1)
        exitProc(-1);

    if ((max_memory = satoi(argv[0])) <= 0)
        exitProc(-1);

    while (1)
    {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory)
        {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = allocM(mm_rqs[rq].size);

            if (mm_rqs[rq].address)
            {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                setMem(mm_rqs[i].address, i, mm_rqs[i].size);

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
                {
                    printf("test_mm ERROR\n");
                    exitProc(-1);
                }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                freeM(mm_rqs[i].address);
    }
    return 0;
}