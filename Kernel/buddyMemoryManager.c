// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MemoryManagerADT.h"

#define SWAP_BLOCKS(block1, block2) \
    struct Block *temp = block1;    \
    block1 = block2;                \
    block2 = temp;

static MemoryManagerADT kernel_mm;

struct Block
{
    size_t size;
    struct Block *next;
};

struct MemoryManagerCDT
{
    struct Block *freeLists[TOTAL_POWERS_OF_2];
    void *mem_start;
    size_t total_size;
    size_t free_mem;
    size_t occupied_mem;
};

static size_t roundUpToPowerOf2(size_t size)
{
    size_t power = 1;
    while (power < size)
        power *= 2;
    return power;
}

static size_t log2(size_t x)
{
    int result = 0;
    while (x >= 2)
    {
        x /= 2;
        result++;
    }

    return result;
}

// Inserta un bloque de memoria ordenadamente en una lista de bloques libres
static void insertBlock(MemoryManagerADT memoryManager, struct Block *block, int index)
{
    if (block == NULL || index < 0 || index >= TOTAL_POWERS_OF_2)
    {
        return;
    }

    struct Block *current = memoryManager->freeLists[index];
    struct Block *prev = NULL;

    while (current != NULL && ((char *)current < (char *)block))
    {
        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        block->next = current;
        memoryManager->freeLists[index] = block;
    }
    else
    {
        prev->next = block;
        block->next = current;
    }
}

// Esta función asume que los bloques son del mismo tamaño
static int areBuddies(struct Block *block1, struct Block *block2, size_t size)
{
    // Me quedo en block1 con el más chico
    if ((char *)block1 > (char *)block2)
    {
        SWAP_BLOCKS(block1, block2)
    }

    // Si están a size de diferencia => son buddies
    return (((char *)block2 - (char *)block1) == size);
}

/**
 * Busca el buddy de un bloque en una lista de bloques libres. Si lo encuentra, lo saca de la lista.
 * @return el puntero al buddy si lo encuentra, NULL si no
 */
static struct Block *findBuddy(MemoryManagerADT memoryManager, struct Block *block, int index)
{
    if (index < 0 || index > TOTAL_POWERS_OF_2 - 1)
    {
        return NULL;
    }
    struct Block *current = memoryManager->freeLists[index];
    struct Block *prev = NULL;
    int foundBuddy = 0;
    while (current != NULL && !foundBuddy)
    {
        if (areBuddies(block, current, block->size))
        {
            foundBuddy++;
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }

    if (foundBuddy)
    {
        if (prev == NULL)
        {
            memoryManager->freeLists[index] = current->next;
        }
        else
        {
            prev->next = current->next;
        }
    }

    return current;
}

static void declareFreeMem(MemoryManagerADT mm, size_t size)
{
    mm->free_mem = mm->free_mem + size;
    mm->occupied_mem = mm->occupied_mem - size;
}

static void declareAllocMem(MemoryManagerADT mm, size_t size)
{
    mm->free_mem = mm->free_mem - size;
    mm->occupied_mem = mm->occupied_mem + size;
}

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory)
{
    kernel_mm = (MemoryManagerADT)memoryForMemoryManager;
    kernel_mm->mem_start = managedMemory;
    kernel_mm->total_size = TOTAL_MEM;

    // Todas las listas de bloques libres están vacías excepto la última
    for (int i = 0; i < TOTAL_POWERS_OF_2 - 1; i++)
    {
        kernel_mm->freeLists[i] = NULL;
    }
    // La última tiene 1 bloque ocupando toda la memoria
    kernel_mm->freeLists[TOTAL_POWERS_OF_2 - 1] = (struct Block *)kernel_mm->mem_start;
    kernel_mm->freeLists[TOTAL_POWERS_OF_2 - 1]->size = TOTAL_MEM;
    kernel_mm->freeLists[TOTAL_POWERS_OF_2 - 1]->next = NULL;

    kernel_mm->free_mem = TOTAL_MEM;
    kernel_mm->occupied_mem = 0;
    return kernel_mm;
}

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate)
{
    if (memoryToAllocate == 0 || (memoryToAllocate + sizeof(struct Block)) > memoryManager->free_mem)
    {
        return NULL;
    }

    size_t totalSize = roundUpToPowerOf2(memoryToAllocate + sizeof(struct Block));
    size_t firstPotentialListIndex = log2(totalSize);
    int foundList = -1;
    struct Block *toReturn = NULL;

    // Busco el primer bloque que pueda contener el tamaño pedido
    for (size_t i = firstPotentialListIndex; i < TOTAL_POWERS_OF_2 && toReturn == NULL; i++)
    {
        if (memoryManager->freeLists[i] != NULL)
        {
            // Me quedo con el primer bloque de la lista, sacándolo de la misma
            toReturn = memoryManager->freeLists[i];
            memoryManager->freeLists[i] = toReturn->next;
            foundList = i;
        }
    }

    // Si encontré un bloque, lo divido hasta que tenga el tamaño exacto
    if (toReturn)
    {
        while (toReturn->size > totalSize)
        {
            // Lo divido en 2
            size_t newSize = toReturn->size / 2;
            struct Block *buddy = (struct Block *)((char *)toReturn + newSize);
            buddy->size = newSize;
            toReturn->size = newSize;

            // Ubico a su buddy en la lista anterior, actualizando foundList
            insertBlock(memoryManager, buddy, --foundList);
        }

        declareAllocMem(memoryManager, toReturn->size);
    }

    return toReturn == NULL ? NULL : (void *)((char *)toReturn + sizeof(struct Block));
}

void freeMemory(MemoryManagerADT mm, void *ptr)
{
    if (ptr == NULL || (ptr < mm->mem_start || ptr >= (mm->mem_start + mm->total_size)))
    {
        return;
    }

    // Agarro el bloque de memoria justo detrás del puntero
    struct Block *toAdd = (struct Block *)((char *)ptr - sizeof(struct Block));
    size_t sizeToFree = toAdd->size;
    int index = log2(toAdd->size);
    int buddyCouldExist = 1;

    // En la lista adecuada, chequeo si está su buddy
    while (buddyCouldExist)
    {
        struct Block *buddy = findBuddy(mm, toAdd, index);

        if (buddy)
        {
            // Si está, los uno y sigo buscando en la próxima lista
            if ((char *)toAdd > (char *)buddy)
            {
                SWAP_BLOCKS(toAdd, buddy)
            }
            toAdd->size *= 2;
            index++;
        }
        else
        {
            // Si no, inserto toAdd en la lista y corto
            insertBlock(mm, toAdd, index);
            buddyCouldExist = 0;
        }
    }

    declareFreeMem(mm, sizeToFree);
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
    MemStatus *mem_status = allocMemoryKernel(sizeof(MemStatus));
    mem_status->total_mem = kernel_mm->total_size;
    mem_status->free_mem = kernel_mm->free_mem;
    mem_status->occupied_mem = kernel_mm->occupied_mem;
    return mem_status;
}