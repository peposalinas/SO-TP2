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
} pipe_t;

pipe_t *getPipe(int id);
int createPipe(int id);

#endif