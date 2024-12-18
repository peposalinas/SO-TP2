// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "test_util.h"
#include "libc.h"

int test_mem(int argc, char *argv[])
{
    MemStatus *mem = memStatus();
    if (mem == NULL)
    {
        printf("Error getting memory status\n");
        exitProc(1);
        return 1;
    }
    int32_t freeMemo = mem->free_mem / (1024 * 1024);
    int32_t totalMemo = mem->total_mem / (1024 * 1024);
    int32_t occupiedMemo = mem->occupied_mem / (1024 * 1024);
    printf("Total mem: %d MB\nOccupied mem: %d MB\nFree mem: %d MB\n", totalMemo, occupiedMemo, freeMemo);
    freeM(mem);

    int *wasteSomeMem = allocM(1024 * 1024 * 10);
    if (wasteSomeMem == NULL)
    {
        printf("Error allocating memory\n");
        exitProc(1);
    }
    printf("\n\nWasted 10 MB\n\n");
    mem = memStatus();
    freeMemo = mem->free_mem / (1024 * 1024);
    totalMemo = mem->total_mem / (1024 * 1024);
    occupiedMemo = mem->occupied_mem / (1024 * 1024);
    printf("Total mem: %d MB\nOccupied mem: %d MB\nFree mem: %d MB\n", totalMemo, occupiedMemo, freeMemo);

    printf("\n\nNow freeing the wasted memory\n\n");
    freeM(wasteSomeMem);
    mem = memStatus();
    freeMemo = mem->free_mem / (1024 * 1024);
    totalMemo = mem->total_mem / (1024 * 1024);
    occupiedMemo = mem->occupied_mem / (1024 * 1024); // CHEQUEAR SACAR EL \t DE ABAJO (FINAL DEL PRINTF)
    printf("Total mem: %d MB\nOccupied mem: %d MB\nFree mem: %d MB\n", totalMemo, occupiedMemo, freeMemo);
    exitProc(0);
    return 0;
}