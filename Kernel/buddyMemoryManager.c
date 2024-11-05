#include "MemoryManagerADT.h"

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
    size_t total_size;
    size_t continuous_free_space;
    size_t free_mem;
    size_t occupied_mem;
};

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

static size_t roundUpToPowerOf2(size_t size)
{
    size_t power = 1;
    while (power < size)
        power *= 2;
    return power;
}

static size_t getBlockSize(void *block, void *mem_start)
{
    return ((struct Block *)block)->size;
}

static void *getBuddy(void *block, size_t size, void *mem_start)
{
    size_t diff = (char *)block - (char *)mem_start;
    return (void *)((char *)mem_start + (diff ^ size));
}

static int isBuddy(void *block1, void *block2, size_t size, void *mem_start)
{
    return block2 == getBuddy(block1, size, mem_start);
}

void *allocMemory(MemoryManagerADT mm, size_t memoryToAllocate)
{
    if (memoryToAllocate == 0 || memoryToAllocate > mm->free_mem)
        return NULL;

    // Redondeo a la potencia de 2 más cercana y agrego el tamaño de un struct block
    size_t total_size = roundUpToPowerOf2(memoryToAllocate + sizeof(struct Block));
    struct Block *current = mm->free_blocks;
    struct Block *prev = NULL;

    // Encuentro un bloque que pueda contener el tamaño solicitado
    while (current != NULL)
    {
        if (current->size >= total_size)
            break;
        prev = current;
        current = current->next;
    }

    if (current == NULL)
        return NULL;

    // Divido bloques hasta encontrar el tamaño adecuado
    while (current->size > total_size)
    {
        size_t new_size = current->size / 2;
        struct Block *buddy = (struct Block *)((char *)current + new_size);
        buddy->size = new_size;
        buddy->next = current->next;
        current->size = new_size;
        current->next = buddy;
    }

    // Saco el bloque de la lista de bloques libres
    if (prev)
        prev->next = current->next;
    else
        mm->free_blocks = current->next;

    mm->free_mem -= current->size;
    mm->occupied_mem += current->size;

    return (void *)((char *)current + sizeof(struct Block));
}

void freeMemory(MemoryManagerADT mm, void *ptr)
{
    if (ptr == NULL)
        return;

    // Agarro el struct block
    struct Block *block = (struct Block *)((char *)ptr - sizeof(struct Block));
    size_t size = block->size;

    mm->free_mem += size;
    mm->occupied_mem -= size;

    // Agrego el bloque a la lista de bloques libres
    struct Block *current = mm->free_blocks;
    struct Block *prev = NULL;

    // Encuentro el lugar donde insertar el bloque (ordenado por dirección)
    while (current != NULL && current < block)
    {
        prev = current;
        current = current->next;
    }

    // Lo inserto en la lista
    block->next = current;
    if (prev)
        prev->next = block;
    else
        mm->free_blocks = block;

    // Fusiono buddies si es posible

    int buddyExists = 1;
    while (buddyExists)
    {
        struct Block *buddy = NULL;
        struct Block *buddy_prev = NULL;
        current = mm->free_blocks;
        prev = NULL;

        // Encuentro el buddy del bloque (si existe)
        while (current != NULL)
        {
            if (current != block && isBuddy(block, current, size, mm->mem_start))
            {
                buddy = current;
                buddy_prev = prev;
                break;
            }
            prev = current;
            current = current->next;
        }

        // Si no se encuentra un buddy o son de tamaños distintos, termino
        if (!buddy || buddy->size != size)
            buddyExists = 0;
        else
        {
            // Remove buddy from list
            if (buddy_prev)
                buddy_prev->next = buddy->next;
            else
                mm->free_blocks = buddy->next;

            // Merge blocks (keep the lower address)
            if (block > buddy)
            {
                struct Block *temp = block;
                block = buddy;
                buddy = temp;
            }

            block->size *= 2;
            size *= 2;
        }
    }
}

void *allocMemoryKernel(size_t memoryToAllocate)
{
    return allocMemory(kernel_mm, memoryToAllocate);
}

void freeMemoryKernel(void *ptr)
{
    freeMemory(kernel_mm, ptr);
}

MemStatus *memStatusKernel()
{
    MemStatus *status = memStatusKernel();
    status->total_mem = kernel_mm->total_size;
    status->free_mem = kernel_mm->free_mem;
    status->occupied_mem = kernel_mm->occupied_mem;
    return status;
}