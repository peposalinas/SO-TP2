/* sampleCodeModule.c */
#include <shell.h>
#include <libc.h>
#include <eliminator.h>
#include <syscaller.h>
#include <stdint.h>
#include <welcome.h>
#include <font.h>

void idleUser();
uint64_t test_sync(uint64_t argc, char *argv[]);
void test_child();
void test_waitPid();
void test_child2();

int main()
{
	// char * argvShell[] = {NULL};
	// while (1)
	// {
	// 	welcome();
	// 	uint8_t *msg2[] = {"Please select a program to run:\0", "1. Eliminator\0", "2. Shell\0"};
	// 	for (uint8_t i = 0; i < 3; i++)
	// 	{
	// 		uint8_t j = 0;
	// 		while (msg2[i][j])
	// 		{
	// 			drawBigChar(msg2[i][j], 82 + j, 2 + i, ORANGE, WHITE);
	// 			j++;
	// 		}
	// 	}
	// 	uint8_t c = 0;
	// 	while (c != '1' && c != '2')
	// 		readCaller(UNUSED, &c, 1);
	// 	switch (c)
	// 	{
	// 	case '1':
	// 		gameMain();
	// 		break;

	// 	case '2':
	// 		createProcess("shell", 4, launchShell, 0, argvShell);
	// 		break;
	// 	}
	// }
	// launchShell();

	int pid = createProcess("shell", 4, launchShell, 0, NULL);
	waitPID(pid);
	// char *argvWait[1] = {NULL};
	// createProcess("test_waitPid", 4, test_waitPid, 1, argvWait);
	exitProc(0);
}

void idleUser()
{
	while (1)
		;
}

void test_waitPid()
{
	char *argvTest[1] = {NULL};
	printf("\nEmpece (parent): %d", getPID());
	printf("\nWaiting for my child");
	int pidChild[4];
	for (size_t i = 0; i < 2; i++)
	{
		pidChild[i] = createProcess("test_child", 4, test_child, 1, argvTest);
		pidChild[i + 2] = createProcess("test_child2", 4, test_child2, 1, argvTest);
	}
	for (size_t i = 0; i < 2; i++)
	{
		int returnV = waitPID(pidChild[i]);
		printf("\nMy child has finished with value: %d", returnV);
		int returnV2 = waitPID(pidChild[i + 2]);
		printf("\nMy child2 has finished with value: %d", returnV2);
	}
	exitProc(0);
}

void test_child()
{
	printf("\nEmpece (child)");
	yieldProcess();
	int j = 0;
	for (int i = 0; i < 5; i++)
	{
		printf("\n%d", i);
		while (j < 100000)
		{
			j++;
		}
		j = 0;
	}
	printf("\nTermine (child)");
	exitProc(100);
}

void test_child2()
{
	printf("\nEmpece (child2)");
	yieldProcess();
	int j = 0;
	for (int i = 6; i < 11; i++)
	{
		printf("\n%d", i);
		while (j < 100000)
		{
			j++;
		}
		j = 0;
	}
	printf("\nTermine (child2)");
	exitProc(80);
}