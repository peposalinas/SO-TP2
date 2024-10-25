#include "./include/test_util.h"
#include "libc.h"
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

void endless_loop()
{
    while (1)
        ;
}

void endless_loop_print(uint64_t wait)
{
    int64_t pid = getPID();

    while (1)
    {
        // bussy_wait(wait);
        // ncPrintDec(pid);
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
