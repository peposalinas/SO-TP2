#ifndef PIPES_H
#define PIPES_H
#include "semaphores.h"

#define MAX_PIPES 256
#define PIPE_SIZE 1024
#define FIRST_SEM_FOR_PIPES 1024
#define LAST_SEM_FOR_PIPES 1024 + MAX_PIPES * 2

#define KEYBOARD_PIPE 0
#define TERMINAL_PIPE 1

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