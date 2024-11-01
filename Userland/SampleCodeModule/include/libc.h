#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>
#include <stdarg.h>
#include <syscaller.h>

#define EOF -1
#define NULL 0

int printf(const char *fmt, ...);
int putChar(char c);
int getChar();
void cleanBuffer();
void scanf(const char *fmt, ...);
int atoi(const char *str);
void itos(int num, char *buffer);
int strcmp(char *s1, char *s2);
void *allocM(int memoryToAllocate);
void freeM(void *ptr);
MemStatus *memStatus();
int createProcess(char *process_name, int process_priority, void (*entry_point)(void), int argc, char *argv[]);
void exitProc(int returnVal);
int getPID();
// void listProcesses();
int killProcess(int pid);
int changeProcessPriority(int pid, int priority);
int blockProcess(int pid);
int unblockProcess(int pid);
void yieldProcess();
int waitPID(int pid);
int openSem(int id, int value);
void closeSem(int id);
void waitSem(int id);
void postSem(int id);
void clearScreen();
char *strtok(char *str, const char *delim);
char *strchr(const char *str, int c);

#endif