#ifndef PIPES_H
#define PIPES_H
#include "semaphores.h"
#include "definitions.h"

typedef struct pipe_t
{
    char buffer[PIPE_SIZE];
    int currentReadPos;
    int currentWritePos;
    int semWrite;
    int semRead;
    int inUse;
    int readers;
    int writers;
} pipe_t;

pipe_t *getPipe(int id);
int createPipe(int id);
int closePipe(int id, int isReader, int closerPid);
int openPipe(int id, int isReader);
int writeToPipe(int pipeId, const uint8_t *string, uint32_t size);
int readFromPipe(int pipeId, uint8_t *buffer, uint32_t size);

#endif