#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>
#include "time.h"
#include "MemoryManagerADT.h"
#include "listADT.h"
#include "processes.h"
#include "lib.h"
#include "naiveConsole.h"
#include "interrupts.h"

// CHEQUEAR: Meter esto en defs.h???
#define QUANTUM_IN_MS 20
#define QTY_PRIORITIES 5
#define MAX_PROCESSES 1000

#define HIGHEST_PRIO 4
#define HIGH_PRIO 3
#define MEDIUM_PRIO 2
#define LOW_PRIO 1
#define LOWEST_PRIO 0

#define DEFAULT_PRIORITY 2

typedef struct processInformation
{
    uint64_t pid;
    processState_t state;
    char *name;
    int priority;
    uint64_t *stack;
    uint64_t *stack_pointer;
    uint64_t parent_pid;

} processInformation;

typedef struct schedulerCDT *schedulerADT;

/*Inicializa los recursos necesarios para la creación del scheduler*/
void schedulerInit();

/**
 * Añade un proceso con la prioridad más alta
 * @param process_name nombre del proceso
 * @param process_priority prioridad del proceso
 * @param entry_point función a ejecutar, del tipo void (*e_p)(void)
 * @param argc cantidad de args
 * @param argv arreglo de args
 * @return pid del proceso agregado
 */
int schedulerAddProcess(char *processName, int processPriority, int (*entry_point)(int, char **), int argc, char **argv, int *pipesIO);

int schedulerAddStandardProcess(char *processName, int processPriority, int (*entry_point)(int, char **), int argc, char **argv);

/**
 * Decide qué proceso correr
 * @param current_stack_pointer SP del proceso actual
 * @return SP del proceso a correr
 */
uint64_t *schedulerRun(uint64_t *currentStackPointer);

/**
 * Quita del scheduler el proceso de pid indicado (no libera sus recursos)
 * @param pid pid del proceso a matar
 * @return pid del proceso si tiene éxito, -1 si no
 */
int schedulerKillProcess(uint32_t pid);

/**
 * Bloquea un proceso
 * @param pid pid del proceso a bloquear
 * @return pid del proceso si tiene éxito, -1 si no
 */
int schedulerBlockProcess(uint32_t pid);

/**
 * Bloquea un proceso
 * @param pid pid del proceso a bloquear
 * @return pid del proceso si tiene éxito, -1 si no
 */
int schedulerUnblockProcess(uint32_t pid);

uint64_t schedulerChangePriority(uint64_t pid, int priority);

uint64_t getRunningPid();

void exitProcess(uint64_t returnVal);

uint64_t waitPid(uint64_t pid);

void listProcesses();

void schedulerYield();

int getCurrentInputPipe();

int getCurrentOutputPipe();

int getCurrentInputPipe();

int getCurrentOutputPipe();

processInformation *getAllProcessesInformation();

#endif