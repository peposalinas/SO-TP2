#include "processes.h"
#include "scheduler.h"
#include "MemoryManagerADT.h"

static uint32_t pid = 1;
static process_base *pb;

static process_t *getProcessRec(process_t *curr_process, uint32_t pid);

int initializeProcessCreation()
{
    pb = allocMemoryKernel(sizeof(process_base));
    CHECK_NOT_NULL(pb)
    pb->first_process = NULL;
    return 1;
}

int createProcess(void (*entry_point)(void), int argc, char *argv[])
{
    process_t *new_process = allocMemoryKernel(sizeof(process_t));
    CHECK_NOT_NULL(new_process)
    new_process->pid = pid;
    pid++;
    new_process->stack = allocMemoryKernel(STACK_SIZE);
    CHECK_NOT_NULL(new_process->stack)
    new_process->stack_pointer = new_process->stack;
    new_process->state = READY;
    new_process->next = NULL;
    // new_process->ms_used = 0;
    new_process->priority = 4; // Prioridad más alta

    // Lo metemos al principio de la lista de procesos
    process aux = pb->first_process;
    pb->first_process = new_process;
    new_process->next = aux;

    // Lo añadimos al scheduler
    schedulerAddProcess(new_process);
    return new_process->pid;
}

int killProcess(uint32_t pid)
{
    process_t *it = pb->first_process;
    int wasRemoved = 0, wasFound = 0;
    while (it != NULL && !wasFound)
    {
        if (it->pid == pid)
        {
            wasFound++;
            it->state = TERMINATED;
            freeMemoryKernel(it->stack);
            wasRemoved = schedulerKillProcess(pid);
        }
        it = it->next;
    }
    return wasRemoved;
}

process_t *getProcess(uint32_t pid)
{
    if (!pb)
    {
        return NULL;
    }
    return getProcessRec(pb->first_process, pid);
}

static process_t *getProcessRec(process_t *curr_process, uint32_t pid)
{
    if (curr_process == NULL || curr_process->pid == pid)
    {
        return curr_process;
    }
    return getProcessRec(curr_process->next, pid);
}