#include "./include/MemoryManagerADT.h"
#include "./include/test_util.h"

/*@TODO: Chequear casteos a (char*)
         Sacar continuous_free_space
*/

static void declareFreeMem(MemoryManagerADT mm, size_t size);
static void declareAllocMemory(MemoryManagerADT mm, size_t size);

static MemoryManagerADT kernel_mm;

struct Block
{
    size_t size;
    struct Block *next;
};

struct MemoryManagerCDT
{
    struct Block *free_blocks;
    void *mem_start;
    int total_size;
    size_t continuous_free_space;
    int free_mem;
    int occupied_mem;
};

/*
@TODO: Modularizar líneas 20-22 / 29-31 (asignado de variables de struct Block)
       Hace falta managedmemory? O usamos solo 1 parámetro?
*/
MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory) //@TODO: Chequear si le tenemos que pasar parámetro size
{
    MemoryManagerADT mm = (MemoryManagerADT)memoryForMemoryManager;
    mm->mem_start = managedMemory;
    mm->total_size = TOTAL_MEM;
    mm->free_blocks = (struct Block *)mm->mem_start;
    mm->free_blocks->size = TOTAL_MEM;
    mm->free_blocks->next = NULL;
    mm->free_mem = TOTAL_MEM;
    mm->occupied_mem = 0;
    kernel_mm = mm;
    return mm;
}

void *allocMemoryKernel(size_t memoryToAllocate)
{
    allocMemory(kernel_mm, memoryToAllocate);
}

void freeMemoryKernel(void *ptr)
{
    freeMemory(kernel_mm, ptr);
}

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate)
{
    if (memoryToAllocate == 0)
    {
        return NULL;
    }

    struct Block *currentBlock = memoryManager->free_blocks;
    struct Block *previousBlock = NULL;

    while (currentBlock != NULL)
    {
        // Verificar si hay suficiente espacio en el bloque actual
        if (currentBlock->size >= memoryToAllocate + sizeof(struct Block))
        {
            // Crear un nuevo bloque
            struct Block *newBlock = (struct Block *)((char *)currentBlock + sizeof(struct Block) + memoryToAllocate);
            newBlock->size = currentBlock->size - sizeof(struct Block) - memoryToAllocate;
            newBlock->next = currentBlock->next;

            // Actualizar el bloque actual
            currentBlock->size = memoryToAllocate;
            currentBlock->next = newBlock;

            // Si no hay bloque anterior, estamos en el primer bloque
            if (previousBlock == NULL)
            {
                memoryManager->free_blocks = currentBlock->next; // Actualiza la cabeza de la lista
            }
            else
            {
                previousBlock->next = currentBlock->next; // Enlaza el bloque anterior al siguiente
            }

            declareAllocMemory(memoryManager, currentBlock->size);

            return (void *)((char *)currentBlock + sizeof(struct Block));
        }

        previousBlock = currentBlock;
        currentBlock = currentBlock->next;
    }

    return NULL;
}

void freeMemory(MemoryManagerADT mm, void *ptr)
{
    if (!ptr || (ptr < mm->mem_start || ptr >= (mm->mem_start + mm->total_size)))
    {
        return;
    }

    struct Block *toAdd = (struct Block *)((char *)ptr - sizeof(struct Block));
    if (toAdd < mm->free_blocks)
    {
        toAdd->next = mm->free_blocks;
        mm->free_blocks = toAdd;
        declareFreeMem(mm, toAdd->size);
    }
    else
    {
        struct Block *current = mm->free_blocks;
        struct Block *previous = NULL;

        while (current != NULL && current < toAdd)
        {
            previous = current;
            current = current->next;
        }

        declareFreeMem(mm, toAdd->size);

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

// char *memStatus(MemoryManagerADT mm)
// {
//     Uso espacio después de todos los bloques
//     char *mem_status = (char *)(mm->mem_start + TOTAL_MEM);
//     sprintf(mem_status, "Free memory: %ld bytes\nOccupied memory: %ld bytes", mm->free_mem, mm->occupied_mem);
//     return mem_status;
// }

MemStatus *memStatusKernel()
{
    MemStatus *mem_status = allocMemoryKernel(sizeof(MemStatus));
    mem_status->total_mem = kernel_mm->total_size;
    mem_status->free_mem = kernel_mm->free_mem;
    mem_status->occupied_mem = kernel_mm->occupied_mem;
    return mem_status;
}

static void declareFreeMem(MemoryManagerADT mm, size_t size)
{
    mm->free_mem = mm->free_mem + size + sizeof(struct Block);
    mm->occupied_mem = mm->occupied_mem - size - sizeof(struct Block);
}

static void declareAllocMemory(MemoryManagerADT mm, size_t size)
{
    mm->free_mem = mm->free_mem - size - sizeof(struct Block);
    mm->occupied_mem = mm->occupied_mem + size + sizeof(struct Block);
}