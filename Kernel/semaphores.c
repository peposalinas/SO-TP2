#include "semaphores.h"

LinkedList *allSemaphores;

void semInit()
{
    allSemaphores = createLinkedList();
}

static bool compareSem(void *s1Ptr, void *idPtr)
{
    sem_t *s1 = (sem_t *)s1Ptr;
    int id = *((int *)idPtr);
    return (s1->id == id);
}

int semOpen(int id, int value)
{
    int idTemp = id;
    sem_t *sem = findElem(allSemaphores, &idTemp, compareSem);

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
    int idTemp = id;
    sem_t *sem = findElem(allSemaphores, &idTemp, compareSem);
    if (sem == NULL)
        return;
    sem->openedCount--;
    if (sem->openedCount == 0)
    {
        destroyLinkedList(sem->blockedPids, _nop);
        removeElem(allSemaphores, &idTemp, compareSem);
        freeMemoryKernel(sem);
    }
}

void semWait(int id)
{
    int idTemp = id;
    sem_t *sem = findElem(allSemaphores, &idTemp, compareSem);
    if (sem == NULL)
        return;

    down(&(sem->mutex));

    if (sem->value == 0)
    {
        intptr_t myPid = getRunningPid();
        insertLast(sem->blockedPids, (void *)myPid);
        up(&(sem->mutex));
        schedulerBlockProcess((int)myPid);
    }
    else
    {
        sem->value--;
        up(&(sem->mutex));
    }
}

void semPost(int id)
{
    int idTemp = id;
    sem_t *sem = findElem(allSemaphores, &idTemp, compareSem);
    if (sem == NULL)
        return;

    down(&(sem->mutex));

    if (sem->blockedPids->size > 0)
    {
        intptr_t nextPid = (intptr_t)removeFirst(sem->blockedPids);
        schedulerUnblockProcess((int)nextPid);
    }
    else
    {
        sem->value++;
    }

    up(&(sem->mutex));
}