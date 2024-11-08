#include "scheduler.h"

typedef struct processList
{
    size_t readyProcessCount;
    LinkedList *processList;
} processList;

typedef struct schedulerCDT
{
    process processes[MAX_PROCESSES];
    processList *priority[QTY_PRIORITIES];
    uint64_t runningProcessPid;
} schedulerCDT;

schedulerADT schedulerKernel;
static int firstTime = 1;

static int checkPID(uint32_t pid);
bool compareProcesses(void *p1, void *p2);

bool compareProcesses(void *p1, void *p2)
{
    return ((process)p1)->pid == ((process)p2)->pid;
}

static int checkPID(uint32_t pid)
{
    if (pid > MAX_PROCESSES || pid < 0 || schedulerKernel->processes[pid] == NULL)
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
        scheduler->priority[i] = allocMemoryKernel(sizeof(processList));
        scheduler->priority[i]->processList = createLinkedList();
        scheduler->priority[i]->readyProcessCount = 0;
    }
    schedulerKernel = scheduler;
    schedulerKernel->runningProcessPid = 0;
    return;
}

int chooseNextPID()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (schedulerKernel->processes[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

int schedulerAddProcess(char *processName, int processPriority, int (*entry_point)(int, char **), int argc, char **argv, int *pipesIO)
{
    process newProcess = (process)allocMemoryKernel(sizeof(process_t));
    int next_pid = chooseNextPID();
    createProcess(newProcess, processName, next_pid, processPriority, entry_point, argc, argv, schedulerKernel->runningProcessPid, pipesIO); // Chequear  si esta bien lo del parent
    schedulerKernel->processes[newProcess->pid] = newProcess;
    insertLast(schedulerKernel->priority[newProcess->priority]->processList, newProcess);
    schedulerKernel->priority[newProcess->priority]->readyProcessCount++;
    return newProcess->pid;
}

int schedulerAddStandardProcess(char *processName, int processPriority, int (*entry_point)(int, char **), int argc, char **argv)
{
    int pipesIO[2] = {KEYBOARD_PIPE, TERMINAL_PIPE};
    return schedulerAddProcess(processName, processPriority, entry_point, argc, argv, pipesIO);
}

uint64_t *schedulerRun(uint64_t *currentStackPointer)
{
    int foundToRun = 0;
    process toRun;
    process current = schedulerKernel->processes[schedulerKernel->runningProcessPid];

    if (firstTime)
    {
        firstTime--;
    }
    else if (current != NULL)
    {
        current->stackPointer = currentStackPointer;
        if (current->state == RUNNING)
        {
            removeFirst(schedulerKernel->priority[current->priority]->processList);
            insertLast(schedulerKernel->priority[current->priority]->processList, current);
            current->state = READY;
        }
    }

    for (int i = QTY_PRIORITIES - 1; i >= 0 && !foundToRun; i--)
    {
        if (schedulerKernel->priority[i]->readyProcessCount > 0)
        {

            toRun = removeFirst(schedulerKernel->priority[i]->processList);
            while (toRun->state != READY)
            {
                insertLast(schedulerKernel->priority[i]->processList, toRun);
                toRun = removeFirst(schedulerKernel->priority[i]->processList);
            }
            insertFirst(schedulerKernel->priority[i]->processList, toRun);
            foundToRun++;
        }
    }

    if (toRun == NULL)
    {
        ncPrint("No hay ningun proceso en el scheduler!!");
        return currentStackPointer;
    }

    schedulerKernel->runningProcessPid = toRun->pid;
    toRun->state = RUNNING;
    return toRun->stackPointer;
}

int schedulerKillProcess(uint32_t pid)
{

    if (checkPID(pid) == -1)
    {
        return -1;
    }

    int priority = schedulerKernel->processes[pid]->priority;
    process toKill = schedulerKernel->processes[pid];

    if (toKill->state == RUNNING || toKill->state == READY)
    {
        schedulerKernel->priority[priority]->readyProcessCount--;
    }

    if (removeElem(schedulerKernel->priority[priority]->processList, toKill, compareProcesses) == NULL)
    {
        return -1;
    }

    killProcess(toKill);
    schedulerKernel->processes[pid] = NULL;

    if (pid == schedulerKernel->runningProcessPid)
    {
        schedulerYield();
    }

    return pid;
}

void exitProcess(uint64_t returnVal)
{
    process toExit = schedulerKernel->processes[schedulerKernel->runningProcessPid];
    toExit->returnValue = returnVal;

    if (toExit->isBeingWaited)
    {
        schedulerUnblockProcess(toExit->parentPid);
    }
    toExit->state = TERMINATED;
    schedulerKernel->priority[toExit->priority]->readyProcessCount--;
    schedulerYield();
    return;
}

uint64_t waitPid(uint64_t pid)
{
    if (checkPID(pid) == -1 || schedulerKernel->processes[pid]->parentPid != schedulerKernel->runningProcessPid)
    {
        return -1;
    }

    schedulerKernel->processes[pid]->isBeingWaited = 1;
    if (schedulerKernel->processes[pid]->state != TERMINATED)
    {
        schedulerBlockProcess(schedulerKernel->runningProcessPid);
    }

    uint64_t return_value = schedulerKernel->processes[pid]->returnValue;
    schedulerKernel->processes[pid]->isBeingWaited = 0;
    schedulerKillProcess(pid);

    return return_value;
}

uint64_t getRunningPid()
{
    return schedulerKernel->runningProcessPid;
}

int schedulerBlockProcess(uint32_t pid)
{

    if (checkPID(pid) == -1 || schedulerKernel->processes[pid]->state == BLOCKED)
    {
        return -1;
    }

    schedulerKernel->processes[pid]->state = BLOCKED;
    schedulerKernel->priority[schedulerKernel->processes[pid]->priority]->readyProcessCount--;
    if (pid == schedulerKernel->runningProcessPid)
    {
        schedulerYield();
    }

    return pid;
}

int schedulerUnblockProcess(uint32_t pid)
{

    if (checkPID(pid) == -1 || schedulerKernel->processes[pid]->state != BLOCKED)
    {
        return -1;
    }

    schedulerKernel->processes[pid]->state = READY;
    schedulerKernel->priority[schedulerKernel->processes[pid]->priority]->readyProcessCount++;

    return pid;
}

uint64_t schedulerChangePriority(uint64_t pid, int priority)
{

    if (checkPID(pid) == -1 || priority < 0 || priority >= QTY_PRIORITIES || schedulerKernel->processes[pid]->state == RUNNING) // VOS DECIS????
    {
        return -1;
    }

    process toChange = schedulerKernel->processes[pid];
    removeElem(schedulerKernel->priority[toChange->priority]->processList, toChange, compareProcesses);
    int previousPrio = toChange->priority;
    toChange->priority = priority;
    insertLast(schedulerKernel->priority[priority]->processList, toChange);

    if (toChange->state == READY)
    {
        schedulerKernel->priority[previousPrio]->readyProcessCount--;
        schedulerKernel->priority[toChange->priority]->readyProcessCount++;
    }

    return pid;
}

processInformation *getAllProcessesInformation()
{
    int totalProcesses = 0;
    for (int i = 0; i < PRIORITY_AMOUNT; i++)
    {
        totalProcesses += schedulerKernel->priority[i]->processList->size;
    }

    processInformation *processes = allocMemoryKernel(sizeof(processInformation) * (totalProcesses + 1));
    int index = 0;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process p = schedulerKernel->processes[i];
        if (p != NULL)
        {
            processes[index].pid = p->pid;
            processes[index].state = p->state;
            processes[index].name = p->name;
            processes[index].priority = p->priority;
            processes[index].stack = p->stack;
            processes[index].stack_pointer = p->stackPointer;
            processes[index].parent_pid = p->parentPid;
            index++;
        }
    }

    processes[index].pid = UINT64_MAX;
    return processes;
}

void schedulerYield()
{
    int20();
}

int getCurrentInputPipe()
{
    return schedulerKernel->processes[schedulerKernel->runningProcessPid]->inputPipe;
}

int getCurrentOutputPipe()
{
    return schedulerKernel->processes[schedulerKernel->runningProcessPid]->outputPipe;
}