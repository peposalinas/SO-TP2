#include "syscall.h"
#include "./include/test_util.h"
#include "./include/processes.h"
#include "./include/scheduler.h"
#include "./include/naiveConsole.h"
#include "stdlib.h"

enum State
{
    RUNNING_T,
    BLOCKED_T,
    KILLED_T
};

typedef struct P_rq
{
    uint64_t pid;
    enum State state;
} p_rq;

int test1(int argc, char *argv[])
{
    while (1)
    {
    }
    return 0;
}

int test2(int argc, char *argv[])
{
    while (1)
    {
    }
    return 0;
}

int64_t test_processes(uint64_t argc, char *argv[])
{
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {0};
    if (argc != 1)
        return -1;

    if ((max_processes = satoi(argv[0])) <= 0)
    {
        return -1;
    }

    p_rq p_rqs[max_processes];
    int i = 0;
    while (1)
    {
        for (rq = 0; rq < max_processes; rq++)
        {
            if (rq % 2 == 0)
            {
                p_rqs[rq].pid = schedulerAddProcess("test1", 4, test1, 0, argvAux);
            }
            else
            {
                p_rqs[rq].pid = schedulerAddProcess("test2", 4, test2, 0, argvAux);
            }

            if (p_rqs[rq].pid == -1)
            {

                ncPrint("test_processes: ERROR creating process\n");
                return -1;
            }
            else
            {
                p_rqs[rq].state = RUNNING_T;
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
                    // ncPrint("Mato");
                    if (p_rqs[rq].state == RUNNING_T || p_rqs[rq].state == BLOCKED_T)
                    {
                        if (schedulerKillProcess(p_rqs[rq].pid) == -1)
                        {
                            ncPrint("test_processes: ERROR killing process\n");
                            return -1;
                        }
                        p_rqs[rq].state = KILLED_T;
                        alive--;
                    }
                    break;

                case 1:
                    // ncPrint("Bloqueo");
                    if (p_rqs[rq].state == RUNNING_T)
                    {
                        if (schedulerBlockProcess(p_rqs[rq].pid) == -1)
                        {
                            ncPrint("test_processes: ERROR blocking process\n");
                            return -1;
                        }
                        p_rqs[rq].state = BLOCKED_T;
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED_T && GetUniform(100) % 2)
                {
                    if (schedulerUnblockProcess(p_rqs[rq].pid) == -1)
                    {
                        ncPrint("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING_T;
                }
        }
    }
}