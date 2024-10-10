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

/**
 * Crea los recursos necesarios para un memory manager. Aunque lo retorna, también lo almacena como variable interna
 * y lo usa en las funciones allocMemoryKernel y freeMemoryKernel.
 */
MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT memoryManager, size_t memoryToAllocate);

void freeMemory(MemoryManagerADT mm, void *ptr);

/**
 * Otorga memoria usando el MM del kernel
 * @param memoryToAllocate cantidad de memoria a asignar
 */
void *allocMemoryKernel(size_t memoryToAllocate);

/**
 * Libera memoria usando el MM dl kernel
 * @param ptr puntero a la memoria a liberar
 */
void freeMemoryKernel(void *ptr);

/**
 *  Refleja la memoria ocupada y libre del Memory Manager (incluyendo los struct block)
 *  @return string representando lo antedicho
 */
char *memStatus(MemoryManagerADT mm); //@TODO: Chequear qué retornamos

#endif