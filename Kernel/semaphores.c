#include "semaphores.h"

LinkedList *allSemaphores;

void semInit()
{
    allSemaphores = createLinkedList();
}

static bool compareSem(void *s1_ptr, void *id_ptr)
{
    sem_t *s1 = (sem_t *)s1_ptr;
    int id = *((int *)id_ptr);
    return (s1->id == id);
}

int semOpen(int id, int value)
{
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);

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
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
    if (sem == NULL)
        return;
    sem->openedCount--;
    if (sem->openedCount == 0)
    {
        destroyLinkedList(sem->blockedPids, _nop);
        removeElem(allSemaphores, &id_temp, compareSem);
        freeMemoryKernel(sem);
    }
}

void semWait(int id)
{
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
    if (sem == NULL)
        return;

    down(&(sem->mutex));

    if (sem->value == 0)
    {
        intptr_t myPid = getRunningPid(); // Use intptr_t to match pointer size
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
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
    if (sem == NULL)
        return;

    down(&(sem->mutex));

    if (sem->blockedPids->size > 0)
    {
        intptr_t nextPid = (intptr_t)removeFirst(sem->blockedPids); // Cast to intptr_t
        schedulerUnblockProcess((int)nextPid);                      // Convert back to int if needed
    }
    else
    {
        sem->value++;
    }

    up(&(sem->mutex));
}