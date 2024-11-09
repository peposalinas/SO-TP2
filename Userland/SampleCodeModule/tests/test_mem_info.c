#include <stdint.h>
#include "test_util.h"
#include "libc.h"

uint64_t test_mem(int argc, char *argv[])
{
    MemStatus *mem = memStatus();
    if (mem == NULL)
    {
        printf("Error getting memory status\n");
        exitProc(1);
    }
    size_t freeMemo = mem->free_mem / (1024 * 1024); // To convert from Bytes to MB
    size_t totalMemo = mem->total_mem / (1024 * 1024);
    size_t occupiedMemo = mem->occupied_mem / (1024 * 1024);
    // Printfs tiran excepcion 8 en modo texto
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
    occupiedMemo = mem->occupied_mem / (1024 * 1024);
    printf("Total mem: %d MB\nOccupied mem: %d MB\nFree mem: %d MB\n", totalMemo, occupiedMemo, freeMemo);
    exitProc(0);
}