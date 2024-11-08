#ifndef SYSCALLER_H
#define SYSCALLER_H

#include <stdint.h>
#include <stddef.h>

#define UNUSED 0

typedef enum
{
    RUNNING,
    READY,
    BLOCKED,
    TERMINATED
} process_state_t;
typedef struct processInformation
{
    uint64_t pid;
    process_state_t state;
    char *name;
    int priority;
    uint64_t *stack;
    uint64_t *stack_pointer;
    uint64_t parent_pid;

} processInformation;
typedef struct MemStatus
{
    size_t total_mem;
    size_t free_mem;
    size_t occupied_mem;
} MemStatus;

uint32_t readCaller(uint8_t __unused, int pipeId, char *buffer, uint32_t size);
long int writeCaller(uint8_t __unused, int pipeId, const char *string, uint32_t size);
void printRectangleCaller(uint8_t __unused, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
uint64_t getScreenWidthCaller(uint8_t __unused);
uint64_t getScreenHeightCaller(uint8_t __unused);
void printCharCaller(uint8_t __unused, uint8_t c, uint32_t x, uint32_t y, uint32_t charColor, uint32_t bgColour);
void waitCaller(uint8_t __unused, int ticks);
uint8_t keyboardStatusCaller(uint8_t __unused);
uint8_t keyboardKeyCaller(uint8_t __unused);
void beepSysCaller(uint8_t __unused, uint32_t frequence, int waitTicks);
uint8_t fontSizeUpCaller(uint8_t __unused);
uint8_t fontSizeDownCaller(uint8_t __unused);
void getTimeCaller(uint8_t __unused, char bp[]);
void *allocMCaller(uint8_t __unused, size_t memoryToAllocate);
void freeMCaller(uint8_t __unused, void *ptr);
// char * memStatusCaller();
int createProcCaller(uint8_t __unused, char *process_name, int (*entry_point)(int, char **), int argc, char *argv[], int *pipesIO);
void exitProcCaller(uint8_t __unused, uint64_t returnVal);
uint64_t getPIDCaller(uint8_t __unused);
int killProcCaller(uint8_t __unused, uint32_t pid);
uint64_t changeProcessPriorityCaller(uint8_t __unused, uint64_t pid, int priority);
int blockProcessCaller(uint8_t __unused, uint32_t pid);
int unblockProcessCaller(uint8_t __unused, uint32_t pid);
void yieldProcessCaller(uint8_t __unused);
uint64_t waitPIDCaller(uint8_t __unused, uint64_t pid);
int openSemCaller(uint8_t __unused, int id, int value);
void closeSemCaller(uint8_t __unused, int id);
void waitSemCaller(uint8_t __unused, int id);
void postSemCaller(uint8_t __unused, int id);

MemStatus *memStatusCaller(uint8_t __unused);
processInformation *listProcessesInfoCaller(uint8_t __unused);
int createStandardProcCaller(uint8_t __unused, char *process_name, int (*entry_point)(int, char **), int argc, char *argv[]);
int getRunningOutputPipeCaller(uint8_t __unused);
int getRunningInputPipeCaller(uint8_t __unused);
int newPipeCaller(uint8_t __unused, int id);
void clearTerminalCaller(uint8_t __unused);

#endif