#include <stdint.h>
#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include "./include/processes.h"
#include "./include/scheduler.h"
#include "./include/naiveConsole.h"
#include "./include/lib.h"
#include "./include/interrupts.h"

#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000     // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

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
    char *argv[] = {0};
    uint64_t i;
    int it = 0;
    while (1)
    {
        it++;
        ncPrintDec(it);
        bussy_wait(WAIT);

        ncClear();
        ncPrint("CREATING PROCESSES...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
            pids[i] = schedulerAddProcess("endless_loop_print", prio[4], endless_loop_print, 0, argv);
        ncClear();
        ncPrint("Got after adding");

        ncClear();
        ncPrint("CHANGING PRIORITIES...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
            schedulerChangePriority(pids[i], prio[i]);

        ncClear();
        ncPrint("BLOCKING...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
            schedulerBlockProcess(pids[i]);

        ncClear();
        ncPrint("CHANGING PRIORITIES WHILE BLOCKED...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
            schedulerChangePriority(pids[i], MEDIUM);

        ncClear();
        ncPrint("UNBLOCKING...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
            schedulerUnblockProcess(pids[i]);

        ncClear();
        ncPrint("KILLING...");
        bussy_wait(WAIT);
        for (i = 0; i < TOTAL_PROCESSES; i++)
        {
            schedulerKillProcess(pids[i]);
        }

        ncClear();
        ncPrint("Got after killin");
        bussy_wait(WAIT);
    }
    exitProcess(0);
}