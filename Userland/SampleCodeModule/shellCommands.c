// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "shellCommands.h"

static char *helpMsg = "PinguinOS - v.5.0\n\n"
                       "block: Block a process\n"
                       "clear: Clear the screen\n"
                       "div0: Divide by zero\n"
                       "exit: Exit the shell\n"
                       "fontBig: Increase the font size\n"
                       "fontSmall: Decrease the font size\n"
                       "help: Show this message\n"
                       "invalidOpCode: Execute an invalid operation code\n"
                       "kill: Kill a process\n"
                       "loop: loop forever and print a salute and its PID\n"
                       "mem: Show memory status\n"
                       "nice: Change the priority of a process\n"
                       "ps: List all processes\n"
                       "testMem: Tests memory allocation\n"
                       "testMemInfo: Tests memory information\n"
                       "testPrio: Tests priorities\n"
                       "testProcesses: Tests processes\n"
                       "testSync: Tests synchronization\n"
                       "cat: Reads from keyboard or from output of piped command and prints\n"
                       "wc: Counts words, lines and characters\n"
                       "filter: Filters vowels\n"
                       "philo: Run the dining philosophers problem\n\n";
;

static int IOPipes[2];
static uint64_t toWaitPID;

void setIOPipes(int iPipe, int oPipe)
{
    IOPipes[0] = iPipe;
    IOPipes[1] = oPipe;
}

void setWaitPID(uint64_t pid)
{
    toWaitPID = pid;
}

int getWaitPID()
{
    return toWaitPID;
}

/*####################### COMMANDS #######################*/

void fontBig(int argc, char *argv[])
{
    fontSizeUpCaller(UNUSED);
    clearTerminalCaller(UNUSED);
}

void div0(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: div0\n");
        return;
    }
    divZero();
}

void fontSmall(int argc, char *argv[])
{
    fontSizeDownCaller(UNUSED);
    clearTerminalCaller(UNUSED);
}

void clearCmd(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: clear\n");
        return;
    }
    clearTerminalCaller(UNUSED);
}

void help(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: help\n");
        return;
    }
    toWaitPID = createProcess("help", helpProc, argc, argv, IOPipes);
}

int helpProc(int argc, char *argv[])
{
    printf("%s", helpMsg);
    exitProc(0);
    return 1;
}

void invalidOpCode(int argc, char *argv[])
{
    opcode();
}

void createTestSync(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: test_sync <n> <useSem>\n");
        return;
    }
    toWaitPID = createProcess("test_sync", test_sync, argc, argv, IOPipes);
}

void createTestMemInfo(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: testMemInfo\n");
        return;
    }
    toWaitPID = createProcess("test_mem", test_mem, argc, argv, IOPipes);
}

void createTestProcesses(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: testProcesses <max_processes>\n");
        return;
    }
    toWaitPID = createProcess("test_processes", test_processes, argc, argv, IOPipes);
}

void createTestPrio(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: testPrio\n");
        return;
    }
    toWaitPID = createProcess("test_prio", test_prio, argc, argv, IOPipes);
}

void createTestMem(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: testMem\n");
        return;
    }
    toWaitPID = createProcess("test_mm", test_mm, argc, argv, IOPipes);
}

void listAllProcesses()
{
    toWaitPID = createProcess("ps", listAllProcessesProc, 0, NULL, IOPipes);
}

int listAllProcessesProc(int argc, char *argv[])
{
    processInformation *toPrint = listProcessesInfo();
    printf("| PID |Priority | State | S.Base           | S.Pointer        | Parent PID | Name\n");

    char stackHex[9];
    char stackPointerHex[9];
    int i = 0;

    while (toPrint[i].pid != UINT64_MAX)
    {
        uint64ToHexString((uint64_t)toPrint[i].stack, stackHex, sizeof(stackHex));
        uint64ToHexString((uint64_t)toPrint[i].stack_pointer, stackPointerHex, sizeof(stackPointerHex));

        printf("|  %d  |    %d    |   %d   |     %s     |     %s     |     %d      | %s\n",
               (int32_t)toPrint[i].pid,
               toPrint[i].priority,
               toPrint[i].state,
               stackHex,
               stackPointerHex,
               (int32_t)toPrint[i].parent_pid,
               toPrint[i].name);
        i++;
    }
    freeM(toPrint);
    exitProc(0);
    return 1;
}

void loop(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: loop <seconds> %s %s\n", argv[0], argv[1]);
        return;
    }
    toWaitPID = createProcess("loopPrinter", loopPrinter, argc, argv, IOPipes);
}

int loopPrinter(int argc, char *argv[])
{
    int seconds = atoi(argv[0]);
    int32_t pid = getPID();
    while (1)
    {
        printf("Hello, my PID is %d\n", pid);
        waitCaller(UNUSED, seconds);
    }
    exitProc(0);
    return 0;
}

void kill(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: kill <pid>\n");
        return;
    }
    int pid = atoi(argv[0]);
    killProcess(pid);
}

void nice(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: nice <pid> <priority>\n");
        return;
    }
    int pid = atoi(argv[0]);
    int priority = atoi(argv[1]);
    changeProcessPriority(pid, priority);
}

void block(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: block <pid>\n");
        return;
    }
    int pid = atoi(argv[0]);
    processInformation *info = listProcessesInfo();
    for (int i = 0; info[i].pid != UINT64_MAX; i++)
    {
        if (info[i].pid == pid)
        {
            if (info[i].state == BLOCKED)
            {
                unblockProcess(pid);
                return;
            }
            else if (info[i].state == READY)
            {
                blockProcess(pid);
                return;
            }
        }
    }
    freeM(info);
}

void memStatusPrinter(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: mem\n");
        return;
    }
    toWaitPID = createProcess("memStatus", memStatusProc, argc, argv, IOPipes);
}

int memStatusProc(int argc, char *argv[])
{
    MemStatus *memStatus = memStatusCaller(UNUSED);
    printf("Total memory: %d B\n", (int32_t)memStatus->total_mem);
    printf("              %d KB\n", (int32_t)memStatus->total_mem / 1024);
    printf("              %d MB\n", (int32_t)memStatus->total_mem / (1024 * 1024));
    printf("\n");
    printf("Free memory: %d B\n", (int32_t)memStatus->free_mem);
    printf("             %d KB\n", (int32_t)memStatus->free_mem / 1024);
    printf("             %d MB\n", (int32_t)memStatus->free_mem / (1024 * 1024));
    printf("\n");
    printf("Occupied memory: %d B\n", (int32_t)memStatus->occupied_mem);
    printf("                 %d KB\n", (int32_t)memStatus->occupied_mem / 1024);
    printf("                 %d MB\n", (int32_t)memStatus->occupied_mem / (1024 * 1024));

    freeM(memStatus);
    exitProc(0);
    return 1;
}

void catCmd(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: cat");
        return;
    }
    toWaitPID = createProcess("cat", catProc, argc, argv, IOPipes);
}

int catProc(int argc, char *argv[])
{
    char c = 0;
    int i = 0;
    char toPrint[BUFF_MAX / 2];
    while (c != ETX && i < BUFF_MAX / 2)
    {
        c = getChar();
        toPrint[i++] = c;
        putChar(c);
    }
    toPrint[i] = 0;
    printf("\n%s\n", toPrint);
    exitProc(0);
    return 1;
}

void wc(int argc, char *argv[])
{
    toWaitPID = createProcess("wc", wcProc, argc, argv, IOPipes);
}

int wcProc(int argc, char *argv[])
{
    char c = 0;
    int inWord = 0;
    int words = 0;
    int lines = 0;
    int chars = 0;

    while ((c = getChar()) != ETX)
    {
        chars++;

        if (c == '\n')
        {
            lines++;
        }
        if (c == ' ' || c == '\n' || c == '\t')
        {
            if (inWord)
            {
                words++;
                inWord = 0;
            }
        }
        else
        {
            inWord = 1;
        }

        putChar(c);
    }
    if (inWord)
    {
        words++;
        lines++;
    }

    printf("\nWords: %d\nLines: %d\nChars: %d\n", words, lines, chars);
    exitProc(0);
    return 1;
}

void filter(int argc, char *argv[])
{
    if (argc != 0)
    {
        printf("Usage: filter\n");
        return;
    }
    toWaitPID = createProcess("filter", filterVowelProc, argc, argv, IOPipes);
}

void philo(int argc, char *argv[])
{
    toWaitPID = createProcess("phylos", philosophersRun, argc, argv, IOPipes);
}

int filterVowelProc(int argc, char *argv[])
{
    char c = 0;
    while ((c = getChar()) != ETX)
    {
        if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u' && c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U')
        {
            putChar(c);
        }
    }
    putChar('\n');
    exitProc(0);
    return 1;
}