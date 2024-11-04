#include <time.h>
#include <stdint.h>
#include <syscalls.h>
#include <keyboardDriver.h>

static void int_20();
static void int_21();

void (*interrupts_arr[])(void) = {int_20, int_21};
void *(*sys_call_arr[])(void *rsi, void *rdx, void *rcx, void *r8, void *r9) = {read /*0*/, write /*1*/, printRectangle /*2*/, getScreenWidth /*3*/, getScreenHeight /*4*/, printChar /*5*/, wait /*6*/, keyboardStatus /*7*/, keyboardKey /*8*/, beepSyscall /*9*/, fontSizeUp /*10*/, fontSizeDown /*11*/, getTime /*12*/, allocM /*13*/, freeM /*14*/, createProc /*15*/, exitProc /*16*/, getPID /*17*/, killProc /*18*/, changeProcPriority /*19*/, blockProc /*20*/, unblockProc /*21*/, yieldProc /*22*/, waitPID /*23*/, openSem /*24*/, closeSem /*25*/, waitSem /*26*/, postSem /*27*/, memStatus /*28*/, listAllProcessesInformation /*29*/};

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
