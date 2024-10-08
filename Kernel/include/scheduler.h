#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "processes.h"
#include <stddef.h>
#include "time.h"
#include "MemoryManagerADT.h"

#define QUANTUM_IN_MS 20
#define PRIORITY_AMOUNT 4

/*Por ahora no hace nada*/
void schedulerInit();

/**
 * Añade un proceso con la prioridad más alta
 * @param proc puntero a un struct process_t
 */
void schedulerAddProcess(process_t *proc);

/**
 * Decide qué proceso correr
 * @param current_stack_pointer SP del proceso actual
 * @return SP del proceso a correr
 */
uint32_t *schedulerRun(uint32_t *current_stack_pointer);

/**
 * Quita del scheduler el proceso de pid indicado (no libera sus recursos)
 * @param pid pid del proceso a matar
 * @return 0 si no pudo matarlo, 1 caso contrario
 */
int schedulerKillProcess(uint32_t pid);

#endif