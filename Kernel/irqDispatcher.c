#include <time.h>
#include <stdint.h>
#include <syscalls.h>
#include <keyboardDriver.h>

static void int_20();
static void int_21();

void (*interrupts_arr[])(void) = {int_20, int_21};
void *(*sys_call_arr[])(void *rsi, void *rdx, void *rcx, void *r8, void *r9) = {(void *)read /*0*/, (void *)write /*1*/, (void *)printRectangle /*2*/, (void *)getScreenWidth /*3*/, (void *)getScreenHeight /*4*/, (void *)printChar /*5*/, (void *)wait /*6*/, (void *)keyboardStatus /*7*/, (void *)keyboardKey /*8*/, (void *)beepSyscall /*9*/, (void *)fontSizeUp /*10*/, (void *)fontSizeDown /*11*/, (void *)getTime /*12*/, (void *)allocM /*13*/, (void *)freeM /*14*/, (void *)createProc /*15*/, (void *)exitProc /*16*/, (void *)getPID /*17*/, (void *)killProc /*18*/, (void *)changeProcPriority /*19*/, (void *)blockProc /*20*/, (void *)unblockProc /*21*/, (void *)yieldProc /*22*/, (void *)waitPID /*23*/, (void *)openSem /*24*/, (void *)closeSem /*25*/, (void *)waitSem /*26*/, (void *)postSem /*27*/, (void *)memStatus /*28*/, (void *)listAllProcessesInformation /*29*/, (void *)createStandardProc /*30*/, (void *)getRunningOutputPipe /*31*/, (void *)getRunningInputPipe /*32*/, (void *)newPipe /*33*/, (void *)clearTerminal /*34*/};

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
