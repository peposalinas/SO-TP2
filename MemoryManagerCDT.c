#include "./include/MemoryManagerADT.h"

struct Block
{
    size_t size;
    struct Block *next;
};

struct MemoryManagerCDT
{
    struct Block *free_blocks;
    void *mem_start;
    size_t total_size;
};

//@TODO: modularizar líneas 20-22 / 29-31 (asignado de variables de struct Block)
MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory) //@TODO: Chequear si le tenemos que pasar parámetro size
{
    MemoryManagerADT mm = (MemoryManagerADT)memoryForMemoryManager;
    mm->mem_start = managedMemory;
    mm->total_size = TOTAL_MEM;
    mm->free_blocks = (struct Block *)managedMemory + sizeof(struct MemoryManagerCDT); // + sizeof(struct MemoryManagerCDT) ?
    mm->free_blocks->next = NULL;                                                      // mm->free_blocks + sizeof(struct Block) + BLOCK_SIZE; //+1?
}

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate)
{
    struct Block *currentBlock = memoryManager->free_blocks;

    // if (memoryManager->free_blocks->next != NULL)
    //     memoryManager->free_blocks = (struct Block *)memoryManager->free_blocks->next;
    // else
    //     memoryManager->free_blocks += sizeof(struct Block) + BLOCK_SIZE;
    memoryManager->free_blocks->size = BLOCK_SIZE;

    memoryManager->free_blocks->next = memoryManager->free_blocks + sizeof(struct Block) + BLOCK_SIZE; //+1?

    // return (void *)(blockToGive + sizeof(struct Block));
    return NULL;
}

void freeMemory(MemoryManagerADT mm, void *ptr)
{
    if (!ptr)
    {
        return;
    }
    struct Block *returnedBlock = (struct Block *)ptr - sizeof(struct Block);
    returnedBlock->next = mm->free_blocks;
    mm->free_blocks = returnedBlock;
}

//@TODO: Insertar ordenado en lista

//@TODO: Iterar por lista ordenada

//@TODO: