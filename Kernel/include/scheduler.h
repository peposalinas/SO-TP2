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

#define QUANTUM_IN_MS 20
#define QTY_PRIORITIES 5
#define MAX_PROCESSES 1000
#define HIGHEST_PRIO 4
#define HIGH_PRIO 3
#define MEDIUM_PRIO 2
#define LOW_PRIO 1
#define LOWEST_PRIO 0

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
int schedulerAddProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[]);

/**
 * Decide qué proceso correr
 * @param current_stack_pointer SP del proceso actual
 * @return SP del proceso a correr
 */
uint32_t *schedulerRun(uint32_t *current_stack_pointer);

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

uint64_t wait_pid(uint64_t pid);

void listProcesses();

void listProcessesByPrio();

void schedulerYield();

#endif