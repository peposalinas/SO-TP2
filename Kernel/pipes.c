#include "pipes.h"

#define PIPE_WRONG(id) (pipeArray[id].inUse == 0 || id < 0 || id >= MAX_PIPES)

static int destroyPipe(int id);

pipe_t pipeArray[MAX_PIPES] = {0};
int semId = FIRST_SEM_FOR_PIPES;

uint8_t endOfText = ETX;

int createPipe(int id)
{
    if (pipeArray[id].inUse != 0 || id < 0 || id >= MAX_PIPES)
    {
        return -1;
    }
    pipeArray[id].currentReadPos = 0;
    pipeArray[id].currentWritePos = 0;
    pipeArray[id].semWrite = semId;
    semOpen(semId++, PIPE_SIZE);
    pipeArray[id].semRead = semId;
    semOpen(semId++, 0);
    pipeArray[id].inUse = 1;
    return 0;
}

pipe_t *getPipe(int id)
{
    if (PIPE_WRONG(id))
    {
        return NULL;
    }
    return &pipeArray[id];
}

int openPipe(int id, int isReader)
{
    if (PIPE_WRONG(id))
    {
        return -1;
    }
    if (isReader)
    {
        pipeArray[id].readers++;
    }
    else
    {
        pipeArray[id].writers++;
    }
    return 0;
}

int closePipe(int id, int isReader, int closerPid)
{
    if (PIPE_WRONG(id))
    {
        return -1;
    }
    if (isReader)
    {
        semDeleteWaiter(pipeArray[id].semRead, closerPid);
        pipeArray[id].readers--;
        if (pipeArray[id].readers == 0 && pipeArray[id].writers == 0)
        {
            destroyPipe(id);
        }
    }
    else
    {
        writeToPipe(id, &endOfText, 1);
        pipeArray[id].writers--;
    }
    return 0;
}

int destroyPipe(int id)
{
    if (pipeArray[id].inUse == 0)
    {
        return -1;
    }
    semClose(pipeArray[id].semWrite);
    semClose(pipeArray[id].semRead);
    pipeArray[id].inUse = 0;
    return 0;
}

int writeToPipe(int pipeId, const uint8_t *string, uint32_t size)
{
    pipe_t *pipe = getPipe(pipeId);
    if (pipe == NULL)
    {
        return -1;
    }
    else if (size == 0)
    {
        return 0;
    }

    int written = 0;
    while (written < size)
    {
        semWait(pipe->semWrite);
        pipe->buffer[pipe->currentWritePos++] = *string;
        semPost(pipe->semRead);
        if (pipe->currentWritePos == PIPE_SIZE)
        {
            pipe->currentWritePos = 0;
        }
        string++;
        written++;
    }
    return written;
}

int readFromPipe(int pipeId, uint8_t *buffer, uint32_t size)
{
    pipe_t *pipe = getPipe(pipeId);
    if (pipe == NULL)
    {
        return -1;
    }
    int bytesRead = 0;
    while (bytesRead < size)
    {
        semWait(pipe->semRead);
        *buffer = pipe->buffer[pipe->currentReadPos++];
        semPost(pipe->semWrite);
        if (pipe->currentReadPos == PIPE_SIZE)
        {
            pipe->currentReadPos = 0;
        }
        bytesRead++;
        buffer++;
    }
    return bytesRead;
}