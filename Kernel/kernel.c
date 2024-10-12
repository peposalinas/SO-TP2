#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <audioDriver.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <time.h>
#include "./include/test_util.h"
#include <interrupts.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

int idle(int argc, char *argv[]);
int64_t test_processes(uint64_t argc, char *argv[]);

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
	return getStackBase();
}

int main()
{
	ncClear();
	_cli();
	void *dir1 = (void *)0x600000;
	void *dir2 = (void *)0x700000;
	createMemoryManager(dir1, dir2);
	setTimerTick(1000);
	schedulerInit();
	char *argvAux[2] = {"idle", NULL};
	schedulerAddProcess("idle", 0, idle, 1, argvAux);
	load_idt();
	// process_list firstList = getRunningProcessList();
	process weHopeThisIsIdle = getRunningProcess();
	if (weHopeThisIsIdle == NULL || weHopeThisIsIdle->name == NULL)
	{
		ncPrint("It's null dumbass");
	}
	// char *toPrint = weHopeThisIsIdle->name;

	ncPrint(weHopeThisIsIdle->name);
	while (1)
		;
	//_sti();
	char *argvTestProc[1] = {"100", NULL};
	test_processes(1, argvTestProc);

	//((EntryPoint)sampleCodeModuleAddress)();
	// testMM("100000");
	return 0;
}

int idle(int argc, char *argv[])
{
	while (1)
	{
		idle_asm();
	}
	return 0;
}
