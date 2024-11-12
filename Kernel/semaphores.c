// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

static bool compareInt(void *i1, void *i2)
{
    intptr_t i1_temp = ((intptr_t)i1);
    int i2_temp = *((int *)i2);
    return i1_temp == i2_temp;
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
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
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

void semMutexWait(int id)
{
    semWait(id);
}

void semMutexPost(int id)
{
    int id_temp = id;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
    if (sem == NULL)
        return;

    down(&(sem->mutex));
    if (sem->value < 1)
    {
        up(&(sem->mutex));
        semPost(id);
        return;
    }
    up(&(sem->mutex));
}

void semDeleteWaiter(int semId, int pid)
{
    int id_temp = semId;
    sem_t *sem = findElem(allSemaphores, &id_temp, compareSem);
    if (sem == NULL)
        return;
    int pid_temp = pid;
    removeElem(sem->blockedPids, &pid_temp, compareInt);
}
