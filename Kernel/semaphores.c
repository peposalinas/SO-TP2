#include "semaphores.h"

static bool compareSem(sem_t s1, int id)
{
    return (s1.id == id);
}

int semOpen(int id, int value)
{
    _cli();
    sem_t *sem = findElem(allSemaphores, id, compareSem);
    if (sem == NULL)
    {
        sem = allocMemoryKernel(sizeof(sem_t));
        if (sem == NULL)
        {
            _sti();
            return -1;
        }
        sem->id = id;
        sem->value = value;
        sem->mutex = 1;
        sem->blockedPids = allocMemoryKernel(sizeof(LinkedList));
        sem->openedCount = 1;
        if (sem->blockedPids == NULL)
        {
            _sti();
            return -1;
        }
        insertLast(allSemaphores, sem);
        _sti();
        return 1;
    }
    sem->openedCount++;
    _sti();
    return 0;
}

void semClose(int id)
{
    sem_t *sem = findElem(allSemaphores, id, compareSem);
    if (sem == NULL)
        return;
    _cli();
    sem->openedCount--;
    if (sem->openedCount == 0)
    {
        destroyLinkedList(sem->blockedPids, _nop);
        removeElem(allSemaphores, id, compareSem);
        freeMemoryKernel(sem);
    }
    _sti();
}

void semWait(int id)
{
    sem_t *sem = findElem(allSemaphores, id, compareSem);
    if (sem == NULL)
        return;
    down(&(sem->mutex));
    if (sem->value == 0)
    {
        int myPid = getRunningPid();
        insertLast(sem->blockedPids, myPid);
        schedulerBlockProcess(myPid);
    }
    sem->value--;
    up(&(sem->mutex));
}

void semPost(int id)
{
    sem_t *sem = findElem(allSemaphores, id, compareSem);
    if (sem == NULL)
        return;
    down(&(sem->mutex));
    sem->value++;
    if (sem->value == 1)
    {
        int nextPid = removeFirst(sem->blockedPids);
        schedulerUnblockProcess(nextPid);
    }
    up(&(sem->mutex));
}