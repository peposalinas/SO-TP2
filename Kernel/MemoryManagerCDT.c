#include "MemoryManagerADT.h"

static void declareFreeMem(MemoryManagerADT mm, size_t size);
static void declareAllocMemory(MemoryManagerADT mm, size_t size);

static MemoryManagerADT kernelMemoryManager;

struct Block
{
    size_t size;
    struct Block *next;
};

struct MemoryManagerCDT
{
    struct Block *freeBlocks;
    void *memStart;
    size_t totalSize;
    size_t continuousFreeSpace;
    size_t freeMem;
    size_t occupiedMem;
};

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory)
{
    MemoryManagerADT mm = (MemoryManagerADT)memoryForMemoryManager;
    mm->memStart = managedMemory;
    mm->totalSize = TOTAL_MEM;
    mm->freeBlocks = (struct Block *)mm->memStart;
    mm->freeBlocks->size = TOTAL_MEM;
    mm->freeBlocks->next = NULL;
    mm->freeMem = TOTAL_MEM;
    mm->occupiedMem = 0;
    kernelMemoryManager = mm;
    return mm;
}

void *allocMemoryKernel(size_t memoryToAllocate)
{
    if (memoryToAllocate == 0)
    {
        return NULL;
    }

    struct Block *currentBlock = kernelMemoryManager->freeBlocks;
    struct Block *previousBlock = NULL;

    while (currentBlock != NULL)
    {
        if (currentBlock->size >= memoryToAllocate + sizeof(struct Block))
        {
            struct Block *newBlock = (struct Block *)((char *)currentBlock + sizeof(struct Block) + memoryToAllocate);
            newBlock->size = currentBlock->size - sizeof(struct Block) - memoryToAllocate;
            newBlock->next = currentBlock->next;

            currentBlock->size = memoryToAllocate;
            currentBlock->next = newBlock;

            if (previousBlock == NULL)
            {
                kernelMemoryManager->freeBlocks = currentBlock->next;
            }
            else
            {
                previousBlock->next = currentBlock->next;
            }

            declareAllocMemory(kernelMemoryManager, currentBlock->size);

            return (void *)((char *)currentBlock + sizeof(struct Block));
        }

        previousBlock = currentBlock;
        currentBlock = currentBlock->next;
    }

    return NULL;
}

void freeMemoryKernel(void *ptr)
{
    if (!ptr || (ptr < kernelMemoryManager->memStart || ptr >= (kernelMemoryManager->memStart + kernelMemoryManager->totalSize)))
    {
        return;
    }

    struct Block *toAdd = (struct Block *)((char *)ptr - sizeof(struct Block));
    if (toAdd < kernelMemoryManager->freeBlocks)
    {
        toAdd->next = kernelMemoryManager->freeBlocks;
        kernelMemoryManager->freeBlocks = toAdd;
        declareFreeMem(kernelMemoryManager, toAdd->size);
    }
    else
    {
        struct Block *current = kernelMemoryManager->freeBlocks;
        struct Block *previous = NULL;

        while (current != NULL && current < toAdd)
        {
            previous = current;
            current = current->next;
        }

        declareFreeMem(kernelMemoryManager, toAdd->size);

        if (previous)
        {
            previous->next = toAdd;
        }
        toAdd->next = current;

        if (previous && (char *)previous + sizeof(struct Block) + previous->size == (char *)toAdd)
        {
            previous->size += sizeof(struct Block) + toAdd->size;
            previous->next = toAdd->next;
            toAdd = previous;
        }

        if (current && (char *)toAdd + sizeof(struct Block) + toAdd->size == (char *)current)
        {
            toAdd->size += sizeof(struct Block) + current->size;
            toAdd->next = current->next;
        }
    }
}

// CHEQUEAR (CamelCase)
MemStatus *memStatusKernel()
{
    MemStatus *mem_status = allocMemoryKernel(sizeof(MemStatus));
    mem_status->total_mem = kernelMemoryManager->totalSize;
    mem_status->free_mem = kernelMemoryManager->freeMem;
    mem_status->occupied_mem = kernelMemoryManager->occupiedMem;
    return mem_status;
}

static void declareFreeMem(MemoryManagerADT mm, size_t size)
{
    mm->freeMem = mm->freeMem + size + sizeof(struct Block);
    mm->occupiedMem = mm->occupiedMem - size - sizeof(struct Block);
}

static void declareAllocMemory(MemoryManagerADT mm, size_t size)
{
    mm->freeMem = mm->freeMem - size - sizeof(struct Block);
    mm->occupiedMem = mm->occupiedMem + size + sizeof(struct Block);
}