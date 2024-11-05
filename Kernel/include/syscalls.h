#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include "scheduler.h"
#include "semaphores.h"
#include "MemoryManagerADT.h"

typedef enum FD
{
    STDOUT = 1,
    STDERR = 2
} FD;

uint32_t read(uint8_t *buffer, uint32_t size);                                                 // 0
long int write(FD fd, const uint8_t *string, uint32_t size);                                   // 1
void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t colors); // 2
uint64_t getScreenWidth();                                                                     // 3
uint64_t getScreenHeight();                                                                    // 4
void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor);         // 5
void wait(uint32_t ticks);                                                                     // 6
uint8_t keyboardStatus();                                                                      // 7
uint8_t keyboardKey();                                                                         // 8
void beepSyscall(uint32_t frequence, uint32_t waitTicks);                                      // 9
uint8_t fontSizeUp();                                                                          // 10
uint8_t fontSizeDown();                                                                        // 11
void getTime(uint8_t pb[]);                                                                    // 12
void *allocM(size_t memoryToAllocate);
void freeM(void *ptr);
int createProc(char *process_name, int process_priority, int (*entry_point)(int, char **), int argc, char *argv[]);
void exitProc(uint64_t returnVal);
uint64_t getPID();
char *listAllProcessesInformation();
int killProc(uint32_t pid);
uint64_t changeProcPriority(uint64_t pid, int priority);
int blockProc(uint32_t pid);
int unblockProc(uint32_t pid);
void yieldProc();
uint64_t waitPID(uint64_t pid);
int openSem(int id, int value);
void closeSem(int id);
void waitSem(int id);
void postSem(int id);
MemStatus *memStatus();

#endif