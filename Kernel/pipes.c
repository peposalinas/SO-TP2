#include "pipes.h"

pipe_t *pipeArray[MAX_PIPES] = {NULL};
int id = FIRST_SEM_FOR_PIPES;

int createPipe(int id)
{
    if (pipeArray[id] != NULL)
    {
        return -1;
    }
    pipeArray[id] = (pipe_t *)allocMemoryKernel(sizeof(pipe_t));
    pipeArray[id]->currentReadPos = 0;
    pipeArray[id]->currentWritePos = 0;
    pipeArray[id]->semWrite = semOpen(id++, PIPE_SIZE);
    pipeArray[id]->semRead = semOpen(id++, 0);
    return 0;
}

pipe_t *getPipe(int id)
{
    return pipeArray[id];
}

int destroyPipe(int id)
{
    if (pipeArray[id] == NULL)
    {
        return -1;
    }
    freeMemoryKernel(pipeArray[id]->buffer);
    semClose(pipeArray[id]->semWrite);
    semClose(pipeArray[id]->semRead);
    freeMemoryKernel(pipeArray[id]);
    pipeArray[id] = NULL;
    return 0;
}