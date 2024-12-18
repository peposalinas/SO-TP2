// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <lib.h>
#include <interrupts.h>
#include <naiveConsole.h>

static unsigned long ticks = 0;

void timer_handler()
{
	ticks++;
}

int ticks_elapsed()
{
	return ticks;
}

int seconds_elapsed()
{
	return ticks / 18.1818;
}

void timer_wait(uint32_t second)
{
	_sti();
	int now = seconds_elapsed();
	while (seconds_elapsed() - now < second)
	{
	}
	return;
}
