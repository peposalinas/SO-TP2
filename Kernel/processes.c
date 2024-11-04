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

int createProcess(process memoryForProcess, char *process_name, uint64_t process_pid, int process_priority, void (*entry_point)(void), int argc, char *argv[], uint64_t parent_pid) // veamos si el entry_point puede recibir argumentos
{

    memoryForProcess->pid = process_pid;
    memoryForProcess->state = READY;
    memoryForProcess->stack_end = allocMemoryKernel(STACK_SIZE);
    memoryForProcess->stack = (uint64_t)(memoryForProcess->stack_end + STACK_SIZE) & ~ALIGN;
    stack_frame *stackFrame = memoryForProcess->stack - sizeof(stack_frame);

    stackFrame->ss = 0x0;
    stackFrame->rsp = memoryForProcess->stack;
    stackFrame->rflags = 0x202;
    stackFrame->cs = 0x8;
    stackFrame->rip = entry_point;
    stackFrame->rdi = argc;
    stackFrame->rsi = argv;

    memoryForProcess->stack_pointer = stackFrame;
    memoryForProcess->name = process_name;
    memoryForProcess->priority = process_priority;
    memoryForProcess->parent_pid = parent_pid;
    memoryForProcess->isBeingWaited = 0;

    return memoryForProcess->pid;
}

int killProcess(process process)
{
    freeMemoryKernel(process->stack_end);
    freeMemoryKernel(process);
}
