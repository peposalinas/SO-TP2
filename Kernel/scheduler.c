#include "scheduler.h"

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
    schedulerADT scheduler = allocMemoryKernel(sizeof(schedulerCDT));
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
    scheduler_kernel = scheduler;
    uint32_t running_process_pid = 0;
    return;
}

int chooseNextPID()
{
    for (int i = 1; i < MAX_PROCESSES; i++)
    {
        if (scheduler_kernel->processes[i] == NULL || scheduler_kernel->processes[i]->state == TERMINATED)
        {
            return i;
        }
    }
    return -1;
}

int schedulerAddProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[])
{
    _cli();
    process newProcess = (process)allocMemoryKernel(sizeof(process_t));
    int next_pid = chooseNextPID();
    createProcess(newProcess, process_name, next_pid, process_priority, entry_point, argc, argv, scheduler_kernel->running_process_pid); // Chequear  si esta bien lo del parent
    scheduler_kernel->priority[newProcess->priority]->ready_process_count++;
    scheduler_kernel->processes[newProcess->pid] = newProcess;
    insertLast(scheduler_kernel->priority[newProcess->priority]->processList, newProcess);
    _sti();
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
        ncPrint("No hay ningun proceso en el scheduler!!");
        return current_stack_pointer;
    }
    scheduler_kernel->running_process_pid = toRun->pid;
    toRun->state = RUNNING;
    return toRun->stack_pointer;
}

int schedulerKillProcess(uint32_t pid)
{
    _cli();
    if (checkPID(pid) == -1)
    {
        _sti();
        return -1;
    }
    int priority = scheduler_kernel->processes[pid]->priority;
    process toKill = scheduler_kernel->processes[pid];
    if (removeElem(scheduler_kernel->priority[priority]->processList, toKill, compareProcesses) == NULL)
    {
        _sti();
        return -1;
    }
    killProcess(toKill);
    scheduler_kernel->processes[pid] = NULL;
    _sti();
    return pid;
}

int schedulerBlockProcess(uint32_t pid)
{
    _cli();
    if (checkPID(pid) == -1)
    {
        return -1;
    }
    scheduler_kernel->processes[pid]->state = BLOCKED;
    scheduler_kernel->priority[scheduler_kernel->processes[pid]->priority]->ready_process_count--;
    _sti();
    return pid;
}

int schedulerUnblockProcess(uint32_t pid)
{
    _cli();
    if (checkPID(pid) == -1)
    {
        return -1;
    }
    scheduler_kernel->processes[pid]->state = READY;
    scheduler_kernel->priority[scheduler_kernel->processes[pid]->priority]->ready_process_count++;
    _sti();
    return pid;
}

uint64_t schedulerChangePriority(uint64_t pid, int priority)
{
    _cli();
    if (checkPID(pid) == -1 || priority < 0 || priority >= QTY_PRIORITIES || scheduler_kernel->processes[pid]->state == RUNNING) // VOS DECIS????
    {
        _sti();
        return -1;
    }
    process toChange = scheduler_kernel->processes[pid];
    removeElem(scheduler_kernel->priority[toChange->priority]->processList, toChange, compareProcesses);
    toChange->priority = priority;
    insertLast(scheduler_kernel->priority[priority]->processList, toChange);
    _sti();
    return pid;
}

uint64_t getRunningPid()
{
    return scheduler_kernel->running_process_pid;
}

void exitProcess(uint64_t returnVal)
{
    _cli();
    scheduler_kernel->processes[scheduler_kernel->running_process_pid]->state = TERMINATED;
    scheduler_kernel->processes[scheduler_kernel->running_process_pid]->return_value = returnVal;
    scheduler_kernel->priority[scheduler_kernel->processes[scheduler_kernel->running_process_pid]->priority]->ready_process_count--;
    if (scheduler_kernel->processes[scheduler_kernel->running_process_pid]->isBeingWaited == 0)
    {
        schedulerKillProcess(scheduler_kernel->running_process_pid);
        ncPrint("I got here");
        while (1)
            ;
    }
    _sti();
    // asm_timer_tick();
}

uint64_t wait_pid(uint64_t pid)
{

    if (checkPID(pid) == -1 || scheduler_kernel->processes[pid]->parent_pid != scheduler_kernel->running_process_pid)
    {
        return -1;
    }
    scheduler_kernel->processes[scheduler_kernel->running_process_pid]->isBeingWaited = 1;
    while (scheduler_kernel->processes[pid]->state != TERMINATED)
    {
        _nop();
    }
    uint64_t return_value = scheduler_kernel->processes[pid]->return_value;
    scheduler_kernel->processes[pid]->isBeingWaited = 0;
    if (scheduler_kernel->processes[pid]->isBeingWaited == 0)
    {
        schedulerKillProcess(pid);
    }
    return return_value;
}