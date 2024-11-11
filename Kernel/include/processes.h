#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include "MemoryManagerADT.h"
#include "definitions.h"

#define STACK_SIZE 4 * 1024
#define PRIORITY_AMOUNT 5
#define ALIGN 7
#define MAX_PIPES_PER_PROCESS 10

int closePipe(int id, int isReader, int closerPid);
int openPipe(int id, int isReader);

typedef enum
{
    RUNNING,
    READY,
    BLOCKED,
    TERMINATED
} process_state_t;

typedef struct process_t
{
    uint64_t pid;
    process_state_t state;
    uint64_t *stack_end;
    uint64_t *stack;
    uint64_t *stack_pointer;
    char **argv;
    char *name;
    int priority;
    uint64_t parent_pid;
    uint64_t return_value; // Como hacer para que retorne un string o void*
    int isBeingWaited;
    int inputPipe;
    int outputPipe;
} process_t;

typedef process_t *process;

/**
 * Crea un nuevo proceso y lo añade al scheduler.
 *
 * @param entry_point puntero a la función a correr
 * @param argc cantidad del argumentos de entry_point
 * @param argv argumentos de entry_point
 * @return pid del proceso si es exitoso, -1 caso contrario
 */
int createProcess(process memoryForProcess, char *process_name, uint64_t process_pid, int process_priority, int (*entry_point)(int, char **), int argc, char *argv[], uint64_t parent_pid, int *pipesIO);

/**
 * Mata un proceso
 * @param pid pid del proceso a matar
 * @return 1 si pudo matarlo, 0 sino
 */
int killProcess(process process);

/**
 * Devuelve el pipe de entrada de un proceso
 * @param process proceso del que se quiere obtener el pipe de entrada
 * @return pipe de entrada del proceso
 */
int getInputPipe(process process);

/**
 * Devuelve el pipe de salida de un proceso
 * @param process proceso del que se quiere obtener el pipe de salida
 * @return pipe de salida del proceso
 */
int getOutputPipe(process process);

#endif