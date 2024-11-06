#include "processes.h"

typedef struct stack_frame
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} stack_frame;

int createProcess(process memoryForProcess, char *process_name, uint64_t process_pid,
                  int process_priority, int (*entry_point)(int, char **),
                  int argc, char *argv[], uint64_t parent_pid, int *pipesIO)
{
    memoryForProcess->pid = process_pid;
    memoryForProcess->state = READY;
    memoryForProcess->stack_end = allocMemoryKernel(STACK_SIZE);
    memoryForProcess->stack = (uint64_t *)(((uint64_t)(memoryForProcess->stack_end) + STACK_SIZE) & ~ALIGN);
    stack_frame *stackFrame = (stack_frame *)((uint64_t *)memoryForProcess->stack - sizeof(stack_frame) / sizeof(uint64_t));

    stackFrame->ss = 0x0;
    stackFrame->rsp = (uint64_t)memoryForProcess->stack;
    stackFrame->rflags = 0x202;
    stackFrame->cs = 0x8;
    stackFrame->rip = (uint64_t)entry_point;
    stackFrame->rdi = (uint64_t)argc;
    stackFrame->rsi = (uint64_t)argv;

    memoryForProcess->stack_pointer = (uint64_t *)stackFrame;
    memoryForProcess->name = process_name;
    memoryForProcess->priority = process_priority;
    memoryForProcess->parent_pid = parent_pid;
    memoryForProcess->isBeingWaited = 0;
    memoryForProcess->inputPipe = pipesIO[0];
    memoryForProcess->outputPipe = pipesIO[1];

    return memoryForProcess->pid;
}

int killProcess(process process)
{
    freeMemoryKernel(process->stack_end);
    freeMemoryKernel(process);
    freeMemoryKernel(process);
    return 0;
}
