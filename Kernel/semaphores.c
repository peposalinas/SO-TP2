#include "semaphores.h"

LinkedList *allSemaphores;

void semInit()
{
    allSemaphores = createLinkedList();
}

static bool compareSem(sem_t *s1, int id)
{
    return (s1->id == id);
}

int semOpen(int id, int value)
{
    sem_t *sem = findElem(allSemaphores, id, compareSem);

    if (sem == NULL)
    {
        sem = allocMemoryKernel(sizeof(sem_t));
        if (sem == NULL)
        {
            return -1;
        }
        sem->id = id;
        sem->value = value;
        sem->mutex = 1;
        sem->blockedPids = createLinkedList();
        sem->openedCount = 1;
        if (sem->blockedPids == NULL)
        {
            return -1;
        }
        insertLast(allSemaphores, sem);
        return 1;
    }
    sem->openedCount++;
    return 0;
}

void semClose(int id)
{
    sem_t *sem = findElem(allSemaphores, id, compareSem);
    if (sem == NULL)
        return;
    sem->openedCount--;
    if (sem->openedCount == 0)
    {
        destroyLinkedList(sem->blockedPids, _nop);
        removeElem(allSemaphores, id, compareSem);
        freeMemoryKernel(sem);
    }
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
        up(&(sem->mutex));
        schedulerBlockProcess(myPid);
        down(&(sem->mutex));
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
    if (sem->blockedPids->size > 0)
    {
        int nextPid = removeFirst(sem->blockedPids);
        schedulerUnblockProcess(nextPid);
    }
    up(&(sem->mutex));
}