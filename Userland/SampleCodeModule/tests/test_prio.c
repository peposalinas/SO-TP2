#include <stdint.h>
#include "test_util.h"
#include "libc.h"

#define MINOR_WAIT "10000000" // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000        // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 5

#define LOWEST 0
#define LOW 1
#define MEDIUM 2
#define HIGH 3
#define HIGHEST 4

int64_t prio[TOTAL_PROCESSES] = {LOWEST, LOW, MEDIUM, HIGH, HIGHEST};

void test_prio(uint64_t argc, char *argv1[])
{

    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {MINOR_WAIT};
    uint64_t i;

    printf("CREATING PROCESSES...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = createProcess("endless_loop_print", prio[4], endless_loop_print, 1, argv);

    printf("\nCHANGING PRIORITIES...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
        changeProcessPriority(pids[i], prio[i]);

    printf("\nBLOCKING...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
        blockProcess(pids[i]);

    //@TODO: Analizar si cambiar esto porque por nuestra implementacion despues de cambiar la prioridad no se ejecuta ninguno (la shell es de prioridad 4)
    printf("\nCHANGING PRIORITIES WHILE BLOCKED...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
        changeProcessPriority(pids[i], MEDIUM);

    printf("\nUNBLOCKING...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
        unblockProcess(pids[i]);

    printf("\nKILLING...\n");
    for (int i = 0; i < WAIT; i++)
        ;
    for (i = 0; i < TOTAL_PROCESSES; i++)
    {
        killProcess(pids[i]);
    }
    for (int i = 0; i < WAIT; i++)
        ;

    exitProc(0);
}