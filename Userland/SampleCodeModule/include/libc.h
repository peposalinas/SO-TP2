#ifndef _LIBC_H_
#define _LIBC_H_
#define EOF -1
#define NULL 0

#include <stddef.h>
#include <stdint.h>

int printf(const char *fmt, ...);
int putChar(char c);
int getChar();
void cleanBuffer();
void scanf(const char *fmt, ...);
int atoi(const char *str);
void itos(int num, char *buffer);
int strcmp(char *s1, char *s2);
void allocM(size_t memoryToAllocate);
void freeM(void *ptr);
// char * memStatus();
int createProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[]);
void exitProc(uint64_t returnVal);
uint64_t getPID();
// void listProcesses();
int killProcess(uint32_t pid);
uint64_t changeProcessPriority(uint64_t pid, int priority);
int blockProcess(uint32_t pid);
int unblockProcess(uint32_t pid);
void yieldProcess();
uint64_t waitPID(uint64_t pid);

#endif