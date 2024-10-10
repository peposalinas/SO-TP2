#include "scheduler.h"
#include "processes.h"

#define QTY_PRIORITIES 5
#define MAX_PROCESSES 100
#define SCHEDULER_ADDRESS 0x900000 // Chequear

/*Remueve un proceso de la lista de "READY"*/
static process_t *removeFromList(process_t *curr, uint32_t pid, int *gotRemoved);

typedef struct process_node
{
    process process;
    process_node *next_process;
} process_node;

typedef struct process_list
{
    process_node *first;
    process_node *last;
    int len;
} process_list;

typedef struct schedulerCDT
{
    process processes[MAX_PROCESSES];
    process_list *priority[QTY_PRIORITIES];
    uint32_t running_process_pid;
    uint32_t pid;
} schedulerCDT;

schedulerADT scheduler_kernel;

void schedulerInit()
{
    schedulerADT scheduler = (schedulerADT)SCHEDULER_ADDRESS;
    for (size_t i = 0; i < QTY_PRIORITIES; i++)
    {
        scheduler->priority[i]->first = NULL;
        scheduler->priority[i]->last = NULL;
        scheduler->priority[i]->len = 0;
    }
    scheduler->pid = 0;
    scheduler_kernel = scheduler;
    uint32_t running_process_pid = 0;
    return;
}

int schedulerAddProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[])
{
    process newProcess = (process)allocMemoryKernel(sizeof(process_t));
    createProcess(newProcess, process_name, scheduler_kernel->pid, process_priority, entry_point, argc, argv); // Esta bien el pasaje de argv y entrypoint??
    scheduler_kernel->pid++;
    process_node *node = allocMemoryKernel(sizeof(process_node));
    node->process = newProcess;
    node->next_process = NULL;
    if (scheduler_kernel->priority[node->process->priority]->len == 0)
    {
        scheduler_kernel->priority[node->process->priority]->first = node;
        scheduler_kernel->priority[node->process->priority]->last = node;
    }
    else
    {
        scheduler_kernel->priority[node->process->priority]->last->next_process = node;
        scheduler_kernel->priority[node->process->priority]->last = node;
    }
    scheduler_kernel->priority[node->process->priority]->len++;
    scheduler_kernel->processes[node->process->pid] = node->process;
    return newProcess->pid;
}

uint32_t *schedulerRun(uint32_t *current_stack_pointer)
{

    process current = scheduler_kernel->processes[scheduler_kernel->running_process_pid];
    current->stack_pointer = current_stack_pointer;
    current->state = READY;

    for (int i = 0; i < QTY_PRIORITIES; i++)
    {
        if (scheduler_kernel->priority[i]->first != NULL)
        {
            while (scheduler_kernel->priority[i]->first->next_process != NULL && scheduler_kernel->priority[i]->first->next_process->process->state != READY)
            {
            }
        }
    }
}

int schedulerKillProcess(uint32_t pid)
{
}

static process_t *removeFromList(process_t *curr, uint32_t pid, int *gotRemoved)
{
    if (curr == NULL)
    {
        return curr;
    }
    if (curr->pid == pid)
    {
        (*gotRemoved)++;
        return curr->next;
    }
    curr->next = removeFromList(curr->next, pid, gotRemoved);
    return curr;
}