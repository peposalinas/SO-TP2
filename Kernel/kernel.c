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

int idle(int argc, char *argv[]);
int64_t test_processes(uint64_t argc, char *argv[]);
void test_prio(uint64_t argc, char *argvTestPrio[]);

void test_waitpid();
void test_child();

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
	setTimerTick(100000);
	_cli();
	load_idt();

	void *dir1 = (void *)0x600000;
	void *dir2 = (void *)0x700000;

	createMemoryManager(dir1, dir2);
	schedulerInit();
	semInit();

	char *argvIdle[2] = {"idle", NULL};
	schedulerAddProcess("idle", LOWEST_PRIO, idle, 1, argvIdle);
	// schedulerAddProcess("test_waitpid", HIGHEST_PRIO, test_waitpid, 1, argvIdle);

	// Test_processes

	// char *argvTest[2] = {"10", NULL};
	// schedulerAddProcess("test", HIGHEST_PRIO, test_processes, 1, argvTest);

	// Fin Test_processes

	// Test_prio

	// char *argvTestPrio[2] = {"test_prio", NULL};
	// uint64_t test_pid = schedulerAddProcess("test_prio", HIGHEST_PRIO, test_prio, 1, argvTestPrio); // TENEMOS que correrlo en máxima prioridad

	// Fin Test_prio

	// int val = wait_pid(test_pid);
	// ncPrintDec(val);
	char *argvSampleCode[2] = {"shell", NULL};
	uint64_t sample_code_pid = schedulerAddProcess("sampleCodeModule", 2, (EntryPoint)sampleCodeModuleAddress, 1, argvSampleCode);

	_sti();

	//  testMM("100000");

	return 0;
}

int idle(int argc, char *argv[])
{
	int i = 0;
	while (1)
	{
		if (i == 0)
		{
			ncPrint("Idle");
			ncNewline();
			i++;
		}
	};
}

void test_waitpid()
{
	char *argvTest[1] = {NULL};
	ncNewline();
	ncPrint("Empece (parent): ");
	ncPrintDec(getRunningPid());
	ncNewline();
	ncPrint("Waiting for my child");
	int pidChild = schedulerAddProcess("test_child", HIGHEST_PRIO, test_child, 1, argvTest);
	// ncPrintDec(pidChild);
	wait_pid(pidChild);
	ncPrint("My child has finished");
	exitProcess(0);
}

void test_child()
{
	ncNewline();
	ncPrint("Empece (child)");
	schedulerYield();
	ncNewline();
	for (int i = 0; i < 5; i++)
	{
		ncNewline();
		ncPrintDec(i);
		ncNewline();
		bussy_wait(10000000);
	}
	ncPrint("Termine (child)");
	ncNewline();
	exitProcess(0);
}
