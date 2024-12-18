// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <syscalls.h>
#include <interrupts.h>
#include <regsDump.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

static void zero_division(const uint64_t regs[19]);
static void invalid_opcode(const uint64_t regs[19]);

void exceptionDispatcher(uint8_t exception, const uint64_t regs[19])
{
	_sti();
	switch (exception)
	{
	case ZERO_EXCEPTION_ID:
		zero_division(regs);
		break;
	case INVALID_OPCODE_ID:
		invalid_opcode(regs);
		break;
	}
}

static void zero_division(const uint64_t regs[19])
{
	printRegisters("Division by zero exception", regs);
	return;
}

static void invalid_opcode(const uint64_t regs[19])
{
	printRegisters("Invalid opcode exception", regs);
	return;
}
