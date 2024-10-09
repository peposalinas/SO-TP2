#include "MemoryManagerADT.h"

struct Node
{
    struct node *right;
    struct node *left;
    void *value;
};

struct BuddyCDT
{
    void *mem_start;
    struct node *left;
    struct node *right;
    size_t total_size;
    size_t free_mem, occuppied_mem;
};

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory)
{
    MemoryManagerADT mm = (MemoryManagerADT)memoryForMemoryManager;
}

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void freeMemory(MemoryManagerADT mm, void *ptr);

char *memStatus(MemoryManagerADT mm);