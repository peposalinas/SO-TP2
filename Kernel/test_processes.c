#include <stdio.h>
#include "syscall.h"
#include "test_util.h"
#include <scheduler.h>
#include "./include/processes.h"

typedef struct P_rq
{
    int32_t pid;
    process_state_t state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[])
{
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};

    if (argc != 1)
        exitProcess(1);

    if ((max_processes = satoi(argv[0])) <= 0)
        exitProcess(1);

    p_rq p_rqs[max_processes];

    int i = 0;
    while (1)
    {
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++)
        {
            p_rqs[rq].pid = schedulerAddProcess("endless_loop", 4, endless_loop, 0, argvAux);

            if (p_rqs[rq].pid == -1)
            {
                ncPrint("test_processes: ERROR creating process\n");
                exitProcess(1);
            }
            else
            {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0)
        {

            for (rq = 0; rq < max_processes; rq++)
            {
                action = GetUniform(100) % 2;

                switch (action)
                {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
                    {
                        if (schedulerKillProcess(p_rqs[rq].pid) == -1)
                        {
                            ncPrint("test_processes: ERROR killing process\n");
                            exitProcess(1);
                        }
                        p_rqs[rq].state = TERMINATED;
                        alive--;
                    }
                    break;

                case 1:
                    if (p_rqs[rq].state == RUNNING)
                    {
                        if (schedulerBlockProcess(p_rqs[rq].pid) == -1)
                        {
                            ncPrint("test_processes: ERROR blocking process\n");
                            exitProcess(1);
                        }
                        p_rqs[rq].state = BLOCKED;
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
            {

                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
                {
                    if (schedulerUnblockProcess(p_rqs[rq].pid) == -1)
                    {
                        ncPrint("test_processes: ERROR unblocking process\n");
                        exitProcess(1);
                    }
                    p_rqs[rq].state = RUNNING;
                }
            }
        }
        ncPrintDec(i);
        i++;
    }
}
