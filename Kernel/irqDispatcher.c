#include <time.h>
#include <stdint.h>
#include <syscalls.h>
#include <keyboardDriver.h>

static void int_20();
static void int_21();

void (*interrupts_arr[])(void) = {int_20, int_21};
void *(*sys_call_arr[])(void *rsi, void *rdx, void *rcx, void *r8, void *r9) = {(void *)read /*0*/, (void *)write /*1*/, (void *)printRectangle /*2*/, (void *)getScreenWidth /*3*/, (void *)getScreenHeight /*4*/, (void *)printChar /*5*/, (void *)wait /*6*/, (void *)keyboardStatus /*7*/, (void *)keyboardKey /*8*/, (void *)beepSyscall /*9*/, (void *)fontSizeUp /*10*/, (void *)fontSizeDown /*11*/, (void *)getTime /*12*/};

void irqDispatcher(uint64_t irq)
{
	interrupts_arr[irq]();
}

void sysCallDispatcher(uint64_t id, void *rsi, void *rdx, void *rcx, void *r8, void *r9)
{
	sys_call_arr[id](rsi, rdx, rcx, r8, r9);
}

void int_20()
{
	timer_handler();
}

void int_21()
{
	keyboard_handler();
}
