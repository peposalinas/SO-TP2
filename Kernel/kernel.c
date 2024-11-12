// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <keyboardDriver.h>
#include <audioDriver.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <time.h>
#include "./include/test_util.h"
#include <interrupts.h>
#include <scheduler.h>
#include "semaphores.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

static int idle(int argc, char *argv[]);

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	ncClear();
	return getStackBase();
}

int main()
{
	_cli();
	load_idt();

	void *dir1 = (void *)0x600000;
	void *dir2 = (void *)0x700000;

	createMemoryManager(dir1, dir2);
	schedulerInit();
	semInit();
	keyboardInit();
	terminalInit();

	char *argvIdle[2] = {"idle", NULL};
	schedulerAddStandardProcess("idle", LOWEST_PRIO, idle, 1, argvIdle);

	char *argvTerminal[2] = {"terminal", NULL};
	int terminalPipes[2] = {TERMINAL_PIPE, TERMINAL_PIPE};
	uint64_t terminal_pid = schedulerAddProcess("terminal", DEFAULT_PRIORITY, (EntryPoint)terminalProcess, 1, argvTerminal, terminalPipes);
	char *argvSampleCode[2] = {"sampleCodeModule", NULL};
	uint64_t sample_code_pid = schedulerAddStandardProcess("sampleCodeModule", DEFAULT_PRIORITY, (EntryPoint)sampleCodeModuleAddress, 1, argvSampleCode);
	_sti();
	wait_pid(terminal_pid);
	wait_pid(sample_code_pid);

	return 0;
}

int idle(int argc, char *argv[])
{
	while (1)
	{
	};
	return 0;
}
