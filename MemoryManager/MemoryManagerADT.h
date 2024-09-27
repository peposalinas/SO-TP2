#ifndef MEMORY_MANAGER_ADT_H
#define MEMORY_MANAGER_ADT_H

#include <stdio.h>
#include <stdlib.h>
#include "syscall.h"
#include "test_util.h"

#define KB *1024 * 1024
#define AMOUNT_OF_KB 128
#define TOTAL_MEM AMOUNT_OF_KB KB // @TODO: Cambiar magic number

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void freeMemory(MemoryManagerADT mm, void *ptr);

/*
    @Returns: el estado actual de la memoria manejada por el Memory Manager
    (memoria ocupada y libre)
*/
char *memStatus(MemoryManagerADT mm); //@TODO: Chequear qué retornamos

#endif