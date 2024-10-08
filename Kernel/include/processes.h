#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "MemoryManagerADT.h"

#define STACK_SIZE 4 * 1024
#define PRIORITY_AMOUNT 4

#define CHECK_NOT_NULL(ptr) \
    if (ptr == NULL)        \
        return 0;

typedef enum
{
    READY,
    BLOCKED,
    TERMINATED
} process_state_t;

typedef struct process_base
{
    struct process *first_process;
} process_base;

typedef struct process
{
    uint32_t pid;
    process_state_t state;
    uint32_t *stack;         // base stack pointer
    uint32_t *stack_pointer; // stack pointer
    struct process *next;
    // int ms_used;
    int priority;
} process_t;

typedef process_t *process;

/**
 * Debe llamarse antes de crear el 1er proceso.
 * Inicializa los recursos necesarios
 *
 * @return 1 si fue exitoso, 0 sino
 */
int initializeProcessCreation();

/**
 * Crea un nuevo proceso y lo añade al scheduler.
 *
 * @param entry_point puntero a la función a correr
 * @param argc cantidad del argumentos de entry_point
 * @param argv argumentos de entry_point
 * @return pid del proceso si es exitoso, 0 caso contrario
 */
int createProcess(void (*entry_point)(void), int argc, char *argv[]);

/**
 * Mata un proceso
 * @param pid pid del proceso a matar
 * @return 1 si pudo matarlo, 0 sino
 */
int killProcess(uint32_t pid);

/**
 * Retorna un proceso en base a su pid
 * @param pid el pid del proceso
 * @return process_t del proceso
 */
process_t *getProcess(uint32_t pid);

#endif