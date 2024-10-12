#include "scheduler.h"
#include "processes.h"
#include "listADT.h"
#include "./include/naiveConsole.h"

#define QTY_PRIORITIES 5
#define MAX_PROCESSES 100
#define SCHEDULER_ADDRESS 0x900000 // Chequear

//@TODO: arreglar tema pid (cuando llega al máximo valor y se mata un proceso podría reutilizar el valor viejo)

typedef struct process_list
{
    size_t ready_process_count;
    LinkedList *processList;
} process_list;

typedef struct schedulerCDT
{
    process processes[MAX_PROCESSES];       // Inicializar en NULL? (checkPID)
    process_list *priority[QTY_PRIORITIES]; // Vector de listas de procesos
    uint64_t running_process_pid;
    uint64_t pid;
} schedulerCDT;

schedulerADT scheduler_kernel;

static int firstTime = 1;
static int timeBlocking = 1;

static int checkPID(uint32_t pid);
bool compareProcesses(process p1, process p2);

bool compareProcesses(process p1, process p2)
{
    return p1->pid == p2->pid;
}

static int checkPID(uint32_t pid)
{
    if (pid > MAX_PROCESSES || pid < 0 || scheduler_kernel->processes[pid] == NULL)
    {
        return -1;
    }
    return 1;
}

void schedulerInit()
{
    schedulerADT scheduler = (schedulerADT)SCHEDULER_ADDRESS;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        scheduler->processes[i] = NULL;
    }
    for (size_t i = 0; i < QTY_PRIORITIES; i++)
    {
        scheduler->priority[i] = allocMemoryKernel(sizeof(process_list));
        scheduler->priority[i]->processList = createLinkedList();
        scheduler->priority[i]->ready_process_count = 0;
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
    insertLast(scheduler_kernel->priority[newProcess->priority]->processList, newProcess);
    scheduler_kernel->priority[newProcess->priority]->ready_process_count++;
    scheduler_kernel->processes[newProcess->pid] = newProcess;
    return newProcess->pid;
}

uint32_t *schedulerRun(uint32_t *current_stack_pointer)
{

    int foundToRun = 0;
    process toRun;
    process current = scheduler_kernel->processes[scheduler_kernel->running_process_pid];
    if (firstTime)
    {
        firstTime--;
    }
    else
    {
        current->stack_pointer = current_stack_pointer;
    }
    current->state = READY;

    for (int i = QTY_PRIORITIES - 1; i >= 0 && !foundToRun; i--)
    {
        // Si hay algún proceso ready de esta prioridad
        if (scheduler_kernel->priority[i]->ready_process_count > 0)
        {

            toRun = removeFirst(scheduler_kernel->priority[i]->processList);

            // Si el proceso no es el que estábamos corriendo o  lo es y solo ese está ready
            if (toRun->pid != scheduler_kernel->running_process_pid || scheduler_kernel->priority[i]->ready_process_count == 1)
            {
                insertFirst(scheduler_kernel->priority[i]->processList, toRun);
                foundToRun++;
            }
            // Si era el que se estaba corriendo y hay otro proceso distinto ready
            else if (scheduler_kernel->priority[i]->ready_process_count > 1)
            {
                //"Iteramos" hasta encontrar al otro
                insertLast(scheduler_kernel->priority[i]->processList, toRun);
                toRun = removeFirst(scheduler_kernel->priority[i]->processList);
                while (toRun->state != READY)
                {
                    insertLast(scheduler_kernel->priority[i]->processList, toRun);
                    toRun = removeFirst(scheduler_kernel->priority[i]->processList);
                }
                insertFirst(scheduler_kernel->priority[i]->processList, toRun);
                foundToRun++;
            }
        }
    }

    if (toRun == NULL)
    {
        ncPrint("No hay ningún proceso en el scheduler (??)");
    }

    // ncPrint("I am runnin");
    // ncNewline();
    // ncPrint(toRun->name);

    scheduler_kernel->running_process_pid = toRun->pid;
    toRun->state = RUNNING;
    return toRun->stack_pointer;
}

int schedulerKillProcess(uint32_t pid)
{
    if (checkPID(pid) == -1)
    {
        return -1;
    }

    // Saco al proceso de la lista
    int prio = scheduler_kernel->processes[pid]->priority;
    process compareProc = allocMemoryKernel(sizeof(process_t));
    compareProc->pid = pid;
    process toKill = removeElem(scheduler_kernel->priority[prio]->processList, compareProc, compareProcesses);
    freeMemoryKernel(compareProc);
    killProcess(toKill);
    return pid;
}

int schedulerBlockProcess(uint32_t pid)
{
    ncPrintDec(timeBlocking);
    timeBlocking++;
    if (checkPID(pid) == -1)
    {
        ncPrintDec(timeBlocking);
        ncPrintDec(pid);
        while (1)
            ;
        return -1;
    }
    scheduler_kernel->processes[pid]->state = BLOCKED;
    return pid;
}

int schedulerUnblockProcess(uint32_t pid)
{
    if (checkPID(pid) == -1)
    {
        return -1;
    }
    scheduler_kernel->processes[pid]->state = READY;
    return pid;
}

process getRunningProcess()
{
    return scheduler_kernel->priority[0]->processList->head->data;
}