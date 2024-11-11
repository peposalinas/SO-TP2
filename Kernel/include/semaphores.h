#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>
#include "listADT.h"
#include "MemoryManagerADT.h"
#include "interrupts.h"
#include "lib.h"
#include "scheduler.h"

typedef struct sem_t
{
    int id;
    int value;
    uint8_t mutex;
    LinkedList *blockedPids;
    int openedCount;
} sem_t;

void semInit();
int semOpen(int id, int value);
void semClose(int id);
void semWait(int id);
void semPost(int id);
void semDeleteWaiter(int semId, int pid);

#endif