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
    size_t continuous_free_space;
};

//@TODO: modularizar líneas 20-22 / 29-31 (asignado de variables de struct Block)
MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory) //@TODO: Chequear si le tenemos que pasar parámetro size
{
    MemoryManagerADT mm = (MemoryManagerADT)memoryForMemoryManager;
    mm->mem_start = managedMemory;
    mm->total_size = TOTAL_MEM;

    mm->free_blocks = (struct Block *)(memoryForMemoryManager + sizeof(struct MemoryManagerCDT)); // @TODO: chequear
    mm->free_blocks->size = TOTAL_MEM;
    mm->free_blocks->next = NULL;
}

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate)
{
    if (memoryToAllocate == 0)
    {
        return NULL;
    }

    // iterar por la lista de bloques libres hasta encontrar bloques con suficiente tamaño
    int found = 0;
    struct Block *currentBlock = memoryManager->free_blocks;
    struct Block *previousBlock = currentBlock;
    do
    {
        if (currentBlock->size > memoryToAllocate + sizeof(struct Block)) // Para simplificar, si chequeamos después se divide en más casos
        {
            // Sacar currentBlock de la lista de bloques libres
            // Que el anterior apunte a currentBlock + sizeof(struct Block) + memoryToAllocate
            previousBlock->next = currentBlock + sizeof(struct Block) + memoryToAllocate; // Asegurado que prevBlock->next tiene espacio disponible
            previousBlock->next->next = currentBlock->next;
            previousBlock->next->size = currentBlock->size - sizeof(struct Block) - memoryToAllocate;
            found = 1;
        }
        else
        {
            // Paso al siguiente bloque
            previousBlock = currentBlock;
            currentBlock = currentBlock->next;
        }

    } while (!found);

    return (void *)currentBlock + sizeof(struct Block);
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