// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include <shell.h>
#include <libc.h>
#include <syscaller.h>
#include <stdint.h>
#include <font.h>
#include "test_util.h"

int main()
{
	int pid = createStandardProc("shell", launchShell, 0, NULL);
	waitPID(pid);
	exitProc(0);
	return 0;
}