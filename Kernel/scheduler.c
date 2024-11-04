#include "scheduler.h"

typedef struct process_list
{
    size_t ready_process_count;
    LinkedList *processList;
} process_list;

typedef struct schedulerCDT
{
    process processes[MAX_PROCESSES];
    process_list *priority[QTY_PRIORITIES];
    uint64_t running_process_pid;
} schedulerCDT;

schedulerADT scheduler_kernel;

static int firstTime = 1;

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
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (scheduler_kernel->processes[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

int schedulerAddProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[])
{
    process newProcess = (process)allocMemoryKernel(sizeof(process_t));
    int next_pid = chooseNextPID();
    createProcess(newProcess, process_name, next_pid, process_priority, entry_point, argc, argv, scheduler_kernel->running_process_pid); // Chequear  si esta bien lo del parent
    scheduler_kernel->processes[newProcess->pid] = newProcess;
    insertLast(scheduler_kernel->priority[newProcess->priority]->processList, newProcess);
    scheduler_kernel->priority[newProcess->priority]->ready_process_count++;
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
    else if (current != NULL)
    {
        current->stack_pointer = current_stack_pointer;
        if (current->state == RUNNING)
        {
            removeFirst(scheduler_kernel->priority[current->priority]->processList);
            insertLast(scheduler_kernel->priority[current->priority]->processList, current);
            current->state = READY;
        }
    }

    for (int i = QTY_PRIORITIES - 1; i >= 0 && !foundToRun; i--)
    {
        if (scheduler_kernel->priority[i]->ready_process_count > 0)
        {

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

    if (checkPID(pid) == -1)
    {
        return -1;
    }

    int priority = scheduler_kernel->processes[pid]->priority;
    process toKill = scheduler_kernel->processes[pid];

    if (toKill->state == RUNNING || toKill->state == READY)
    {
        scheduler_kernel->priority[priority]->ready_process_count--;
    }

    if (removeElem(scheduler_kernel->priority[priority]->processList, toKill, compareProcesses) == NULL)
    {
        return -1;
    }

    killProcess(toKill);
    scheduler_kernel->processes[pid] = NULL;

    if (pid == scheduler_kernel->running_process_pid)
    {
        schedulerYield();
    }

    return pid;
}

void exitProcess(uint64_t returnVal)
{
    process toExit = scheduler_kernel->processes[scheduler_kernel->running_process_pid];
    toExit->return_value = returnVal;

    if (toExit->isBeingWaited)
    {
        schedulerUnblockProcess(toExit->parent_pid);
    }
    toExit->state = TERMINATED;
    scheduler_kernel->priority[toExit->priority]->ready_process_count--;
    schedulerYield();
    return;
}

uint64_t wait_pid(uint64_t pid)
{
    if (checkPID(pid) == -1 || scheduler_kernel->processes[pid]->parent_pid != scheduler_kernel->running_process_pid)
    {
        return -1;
    }

    scheduler_kernel->processes[pid]->isBeingWaited = 1;
    if (scheduler_kernel->processes[pid]->state != TERMINATED)
    {
        schedulerBlockProcess(scheduler_kernel->running_process_pid);
    }

    uint64_t return_value = scheduler_kernel->processes[pid]->return_value;
    scheduler_kernel->processes[pid]->isBeingWaited = 0;
    schedulerKillProcess(pid);

    return return_value;
}

uint64_t getRunningPid()
{
    return scheduler_kernel->running_process_pid;
}

int schedulerBlockProcess(uint32_t pid)
{

    if (checkPID(pid) == -1 || scheduler_kernel->processes[pid]->state == BLOCKED)
    {
        return -1;
    }

    scheduler_kernel->processes[pid]->state = BLOCKED;
    scheduler_kernel->priority[scheduler_kernel->processes[pid]->priority]->ready_process_count--;
    if (pid == scheduler_kernel->running_process_pid)
    {
        schedulerYield();
    }

    return pid;
}

int schedulerUnblockProcess(uint32_t pid)
{

    if (checkPID(pid) == -1 || scheduler_kernel->processes[pid]->state != BLOCKED)
    {
        return -1;
    }

    scheduler_kernel->processes[pid]->state = READY;
    scheduler_kernel->priority[scheduler_kernel->processes[pid]->priority]->ready_process_count++;

    return pid;
}

uint64_t schedulerChangePriority(uint64_t pid, int priority)
{

    if (checkPID(pid) == -1 || priority < 0 || priority >= QTY_PRIORITIES || scheduler_kernel->processes[pid]->state == RUNNING) // VOS DECIS????
    {
        return -1;
    }

    process toChange = scheduler_kernel->processes[pid];
    removeElem(scheduler_kernel->priority[toChange->priority]->processList, toChange, compareProcesses);
    int previousPrio = toChange->priority;
    toChange->priority = priority;
    insertLast(scheduler_kernel->priority[priority]->processList, toChange);

    if (toChange->state == READY)
    {
        scheduler_kernel->priority[previousPrio]->ready_process_count--;
        scheduler_kernel->priority[toChange->priority]->ready_process_count++;
    }

    return pid;
}

void listProcessesByPrio()
{
    for (int i = 0; i < QTY_PRIORITIES; i++)
    {
        ncPrint("[ ");
        ncPrintDec(i);
        ncPrint(" ]");
        ncPrint("(");
        ncPrintDec(scheduler_kernel->priority[i]->processList->size);
        ncPrint(")");
        process_list *list = scheduler_kernel->priority[i];
        LinkedList *processList = list->processList;
        Node *current = processList->head;
        while (current != NULL)
        {
            process p = current->data;
            ncPrint(" -> ");
            ncPrintDec(p->pid);
            ncPrint(":");
            ncPrint(p->name);
            current = current->next;
        }
        ncNewline();
    }
}

void listProcesses()
{
    // ncNewline();
    // ncPrint(" PID: ");
    // ncPrint(" Name: ");
    // ncPrint(" Priority: ");
    // ncPrint(" State: ");
    // ncPrint(" Stack:");
    // ncPrint(" Stack pointer: ");

    // for (int i = 0; i < MAX_PROCESSES; i++)
    // {
    //     if (scheduler_kernel->processes[i] != NULL)
    //     {
    //         ncNewline();
    //         ncPrintDec(scheduler_kernel->processes[i]->pid);
    //         ncPrint("     ");
    //         ncPrint(scheduler_kernel->processes[i]->name);
    //         ncPrint("     ");
    //         ncPrintDec(scheduler_kernel->processes[i]->priority);
    //         ncPrint("          ");
    //         ncPrintDec(scheduler_kernel->processes[i]->state);
    //         ncPrint("     ");
    //         ncPrintHex((uint64_t)scheduler_kernel->processes[i]->stack);
    //         ncPrint("     ");
    //         ncPrintHex((uint64_t)scheduler_kernel->processes[i]->stack_pointer);
    //         ncPrint("     ");

    //         if (scheduler_kernel->running_process_pid == scheduler_kernel->processes[i]->pid)
    //         {
    //             ncPrint("  *  ");
    //         }

    //         ncNewline();
    //     }
    // }
    process_t processArray[MAX_PROCESSES];
    int index = 0;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (scheduler_kernel->processes[i] != NULL)
        {
            processArray[index] = *(scheduler_kernel->processes[i]);
            index++;
        }
    }

    return processArray;
}

void appendString(char *dest, const char *src)
{
    while (*dest)
    {
        dest++;
    }
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void appendInt(char *dest, int value)
{
    char buffer[20];
    int index = 0;

    if (value < 0)
    {
        buffer[index++] = '-';
        value = -value;
    }

    int startIndex = index;
    do
    {
        buffer[index++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    for (int i = startIndex, j = index - 1; i < j; i++, j--)
    {
        char temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    buffer[index] = '\0';
    appendString(dest, buffer);
}

void appendHex(char *dest, unsigned int value)
{
    char buffer[20];
    int index = 0;

    appendString(dest, "0x");

    do
    {
        int digit = value % 16;
        if (digit < 10)
        {
            buffer[index++] = '0' + digit;
        }
        else
        {
            buffer[index++] = 'A' + (digit - 10);
        }
        value /= 16;
    } while (value > 0);

    for (int i = 0, j = index - 1; i < j; i++, j--)
    {
        char temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }

    buffer[index] = '\0';
    appendString(dest, buffer);
}

char *getAllProcessesInformation()
{
    int amount = 0;

    for (int i = 0; i < PRIORITY_AMOUNT; i++)
    {
        amount += scheduler_kernel->priority[i]->processList->size;
    }

    char *toReturn = allocMemoryKernel(amount * BUFSIZ);
    toReturn[0] = '\0';

    appendString(toReturn, "PID      | Priority   | State     | S. Base          | S. Pointer       | Name\n");
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (scheduler_kernel->processes[i] != NULL)
        {
            appendInt(toReturn, scheduler_kernel->processes[i]->pid);
            appendString(toReturn, "        | ");
            appendInt(toReturn, scheduler_kernel->processes[i]->priority);
            appendString(toReturn, "          | ");
            appendInt(toReturn, scheduler_kernel->processes[i]->state);
            appendString(toReturn, "         | ");
            appendHex(toReturn, scheduler_kernel->processes[i]->stack);
            appendString(toReturn, "         | ");
            appendHex(toReturn, scheduler_kernel->processes[i]->stack_pointer);
            appendString(toReturn, "         | ");
            appendString(toReturn, scheduler_kernel->processes[i]->name);
            appendString(toReturn, "\n");
        }
    }

    return toReturn;
}

void schedulerYield()
{
    int20();
}