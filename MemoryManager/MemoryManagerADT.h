#ifndef MEMORY_MANAGER_ADT_H
#define MEMORY_MANAGER_ADT_H

#include <stdio.h>
#include <stdlib.h>
#include "syscall.h"
#include "test_util.h"

#define KB *1024 * 1024
#define TOTAL_MEM 128 KB // @TODO: Cambiar magic number

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void freeMemory(MemoryManagerADT mm, void *ptr);

// char *memStatus(); // Char pointer?

#endif