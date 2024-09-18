#ifndef MEMORY_MANAGER_ADT_ H
#define MEMORY_MANAGER_ADT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void *freeMemory(); // Que le paso a este

char *memStatus(); // Char pointer?

#endif