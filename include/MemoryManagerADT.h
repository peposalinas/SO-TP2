#ifndef MEMORY_MANAGER_ADT_ H
#define MEMORY_MANAGER_ADT_H

#include <stdio.h>
#include <stdlib.h>

#define TOTAL_MEM 1000 // @TODO: Cambiar magic number
#define BLOCK_SIZE 8   // @TODO: Decidir magic number (tamaño 1 bloque)

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void *freeMemory(); // Que le paso a este

char *memStatus(); // Char pointer?

#endif