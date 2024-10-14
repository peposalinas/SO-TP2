#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "./include/test_util.h"
#include "./include/syscalls.h"
#include "./include/MemoryManagerADT.h"
#include "./include/naiveConsole.h"
#include "./include/scheduler.h"
#include "./include/processes.h"
#include "./include/lib.h"
#include "./include/interrupts.h"

#define MAX_BLOCKS 128
#define TICKS 100

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base);

// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

typedef struct MM_rq
{
    void *address;
    uint32_t size;
} mm_rq;

uint32_t GetUint()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max)
{
    uint32_t u = GetUint();
    return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t memcheck(void *start, uint8_t value, uint32_t size)
{
    uint8_t *p = (uint8_t *)start;
    uint32_t i;
    for (i = 0; i < size; i++, p++)
    {
        if (*p != value)
            return 0;
    }

    return 1;
}

// Parameters
int64_t satoi(char *str)
{
    uint64_t i = 0;
    int64_t res = 0;
    int8_t sign = 1;

    if (!str)
        return 0;

    if (str[i] == '-')
    {
        i++;
        sign = -1;
    }

    for (; str[i] != '\0'; ++i)
    {
        if (str[i] < '0' || str[i] > '9')
            return 0;
        res = res * 10 + str[i] - '0';
    }

    return res * sign;
}

void bussy_wait(uint64_t wait_time)
{
    _cli();
    for (uint64_t i = 0; i < wait_time; i++)
    {
        _nop();
    }
    _sti();
    return;
}

void endless_loop()
{
    while (1)
        ;
}

void endless_loop_print(uint64_t wait)
{
    int64_t pid = getRunningPid();

    while (1)
    {
        // bussy_wait(wait);
        // ncPrintDec(pid);
    }
}

uint64_t testMM(char *c)
{
    void *dir1 = (void *)0x600000;
    void *dir2 = (void *)0x700000;

    MemoryManagerADT mm = createMemoryManager(dir1, dir2);
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;

    if ((max_memory = satoi(c)) <= 0)
    {
        return -1;
    }

    int j = 0;

    ncClear();
    while (1)
    {
        j++;
        timer_wait(TICKS);
        ncPrintDec(j);

        while (rq < MAX_BLOCKS && total < max_memory)
        {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = allocMemory(mm, mm_rqs[rq].size);
            if (mm_rqs[rq].address)
            {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
        {
            if (mm_rqs[i].address)
            {
                memset(mm_rqs[i].address, i, mm_rqs[i].size);
            }
        }

        // Check
        for (i = 0; i < rq; i++)
        {
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
                {
                    return -1;
                }
        }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
            {
                freeMemory(mm, mm_rqs[i].address);
            }
    }
}

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    // Calculate characters for each digit
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    // Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}
