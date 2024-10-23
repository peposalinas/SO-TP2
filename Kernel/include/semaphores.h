#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>
#include "listADT.h"

#define MAX_SEMAPHORES 100

typedef struct sem_t
{
    int id;
    int value;
    uint8_t mutex;
    LinkedList *blockedProcesses;
} sem_t;

LinkedList allSemaphores;

int semOpen(int id, int value);
void semClose(int id);
void semWait(int id);
void semPost(int id);

#endif SEMAPHORES_H