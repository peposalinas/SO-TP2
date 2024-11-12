// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <time.h>
#include <audioDriver.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <pipes.h>

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base);

uint32_t read(int pipeId, uint8_t *buffer, uint32_t size)
{
	return readFromPipe(pipeId, buffer, size);
}

long int write(int pipeId, const uint8_t *string, uint32_t size)
{
	return writeToPipe(pipeId, string, size);
}

void printRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			putPixel(color, x + i, y + j);
		}
	}
}

uint64_t getScreenWidth()
{
	return getWidth();
}

uint64_t getScreenHeight()
{
	return getHeight();
}

void printChar(uint8_t c, uint32_t x, uint32_t y, uint32_t fgcolor, uint32_t bgcolor)
{
	drawchar(c, x, y, fgcolor, bgcolor);
}

void wait(uint32_t tick)
{
	timer_wait(tick);
}

uint8_t keyboardStatus()
{
	return getKeyboardStatus();
}

uint8_t keyboardKey()
{
	return getKeyboardKey();
}

void beepSyscall(uint32_t frequence, uint32_t waitTicks)
{
	beep(frequence, waitTicks);
}

uint8_t fontSizeUp()
{
	return tFontBig();
}

uint8_t fontSizeDown()
{
	return tFontSmall();
}

void getTime(uint8_t pb[])
{
	uint8_t buff[64] = {0};
	uint8_t t;
	uint8_t idx = 0;
	for (int8_t i = 2; i >= 0; i--)
	{
		t = getCurrentTime(i);
		if (i == 2)
		{
			if (t <= 2)
			{
				t += 0x24;
			}
			if (0x20 <= t && t <= 0x22)
			{
				t -= 0x6;
			}
			t -= 0x03;
		}
		if (t % 10 == t)
		{
			pb[idx] = '0';
			idx++;
		}
		uintToBase(t, buff, 16);
		int j;

		for (j = 0; buff[j] != 0; j++)
		{
			pb[idx] = buff[j];
			idx++;
		}
		if (i > 0)
		{
			pb[idx] = ':';
			idx++;
		}
	}
	pb[idx] = 0;
}

static uint32_t uintToBase(uint64_t value, uint8_t *buffer, uint32_t base)
{
	char *p = (char *)buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = (char *)buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void *allocM(size_t memoryToAllocate)
{
	return allocMemoryKernel(memoryToAllocate);
}

void freeM(void *ptr)
{
	freeMemoryKernel(ptr);
}

MemStatus *memStatus()
{
	return memStatusKernel();
}

int createProc(char *process_name, int (*entry_point)(int, char **), int argc, char *argv[], int *pipesIO)
{
	return schedulerAddProcess(process_name, DEFAULT_PRIORITY, entry_point, argc, argv, pipesIO);
}

int createStandardProc(char *process_name, int (*entry_point)(int, char **), int argc, char *argv[])
{
	int pipesIO[2] = {KEYBOARD_PIPE, TERMINAL_PIPE};
	return createProc(process_name, entry_point, argc, argv, pipesIO);
}

void exitProc(uint64_t returnVal)
{
	exitProcess(returnVal);
}

uint64_t getPID()
{
	return getRunningPid();
}

processInformation *listAllProcessesInformation()
{
	return getAllProcessesInformation();
}

int killProc(uint32_t pid)
{
	return schedulerKillProcess(pid);
}

uint64_t changeProcPriority(uint64_t pid, int priority)
{
	return schedulerChangePriority(pid, priority);
}

int blockProc(uint32_t pid)
{
	return schedulerBlockProcess(pid);
}

int unblockProc(uint32_t pid)
{
	return schedulerUnblockProcess(pid);
}

void yieldProc()
{
	schedulerYield();
}

uint64_t waitPID(uint64_t pid)
{
	return wait_pid(pid);
}

int openSem(int id, int value)
{
	if (id >= FIRST_SEM_FOR_PIPES && id <= LAST_SEM_FOR_PIPES)
	{
		return -1;
	}
	return semOpen(id, value);
}

void closeSem(int id)
{
	return semClose(id);
}

void waitSem(int id)
{
	return semWait(id);
}

void postSem(int id)
{
	return semPost(id);
}

void waitSemMutex(int id)
{
	return semMutexWait(id);
}

void postSemMutex(int id)
{
	return semMutexPost(id);
}

void deleteWaiterSem(int semId, int pid)
{
	semDeleteWaiter(semId, pid);
}

int getRunningInputPipe()
{
	return getCurrentInputPipe();
}

int getRunningOutputPipe()
{
	return getCurrentOutputPipe();
}

int newPipe(int id)
{
	return createPipe(id);
}

void clearTerminal()
{
	clearCmd();
}

void setForegroundProcess(uint64_t pid)
{
	setAsForegroundProcess(pid);
}