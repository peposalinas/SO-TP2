#include <stdint.h>
#include "test_util.h"
#include "libc.h"

#define SEM_ID 1 // "sem"
#define TOTAL_PAIR_PROCESSES 1

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc)
{
    uint64_t aux = *p;
    yieldProcess(); // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[])
{
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3)
        exitProc(-1);

    if ((n = satoi(argv[0])) <= 0)
        exitProc(-1);
    if ((inc = satoi(argv[1])) == 0)
        exitProc(-1);
    if ((use_sem = satoi(argv[2])) < 0)
        exitProc(-1);

    printf("We're in the son\n");
    if (use_sem)
        if (openSem(SEM_ID, 1) == -1)
        {
            printf("test_sync: ERROR opening semaphore\n");
            exitProc(-1);
        }

    uint64_t i;
    for (i = 0; i < n; i++)
    {
        if (use_sem)
            waitSem(SEM_ID);
        slowInc(&global, inc);
        if (use_sem)
            postSem(SEM_ID);
        printf("%d\t", global);
    }

    if (use_sem)
        closeSem(SEM_ID);

    exitProc(0);
}

uint64_t test_sync(uint64_t argc, char *argv[])
{ //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
    printf("We're in bitches\n");

    if (argc != 2)
        exitProc(-1);

    char *argvDec[] = {argv[0], "-1", argv[1], NULL};
    char *argvInc[] = {argv[0], "1", argv[1], NULL};

    global = 0;

    uint64_t i;

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
        pids[i] = createProcess("my_process_inc", 4, my_process_inc, 3, argvDec);
        pids[i + TOTAL_PAIR_PROCESSES] = createProcess("my_process_inc", 4, my_process_inc, 3, argvInc);
    }

    printf("Sigo haciendo cs");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
        waitPID(pids[i]);
        waitPID(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("\nFinal value: %d", global);

    exitProc(0);
}