#include <shell.h>

#define BUFF_MAX 4096
#define MAX_ARGS 10

#define WHITE 0x00FFFFFF
#define BLACK 0

#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254

#define LETTERS 'z' - 'a' + 1
#define WORDS 5
// ###################################################################
// definiciones para los commandos
typedef struct command_t
{
    char *name;
    void (*function)(uint64_t argc, char *argv[]);
} command_t;

int test_wait_shell(int argc, char *argv[]);
void createTestWaitShell(int argc, char *argv[]);

static void sMoveRight();
static void sMoveLeft();
static void sDeleteChar();
void sCheckCommand();
static void listAllProcesses();
static void clearCmd(int argc, char *argv[]);
static void div0(int argc, char *argv[]);
static void exit(int argc, char *argv[]);
static void fontSmall(int argc, char *argv[]);
static void fontBig(int argc, char *argv[]);
static void getTime(int argc, char *argv[]);
static void help(int argc, char *argv[]);
static void invalidOpCode(int argc, char *argv[]);
static void createTestSync(int argc, char *argv[]);
static void createTestMemInfo(int argc, char *argv[]);
static void createTestProcesses(int argc, char *argv[]);
static void createTestPrio(int argc, char *argv[]);
static void createTestMem(int argc, char *argv[]);
static void loop(int argc, char *argv[]);
static int loopPrinter(int argc, char *argv[]);
static void kill(int argc, char *argv[]);
static void nice(int argc, char *argv[]);
static void block(int argc, char *argv[]);
static void memStatusPrinter(uint64_t argc, char *argv[]);
static int findAndExecCmd(char *cmdName, int argc, char *argv[], int foreground);
static int createNewPipe();
static void catCmd(int argc, char *argv[]);
static int catProc(int argc, char *argv[]);
static char **getArgs(char *buffer);
static int sCheckToken(char *tokens[MAX_ARGS + 1], char *token);

static command_t commands[LETTERS][WORDS] = {{{0, 0}},
                                             {{"block", (void *)block}},
                                             {{"cat", (void *)catCmd}, {"clear", (void *)clearCmd}, {0, 0}},
                                             {{"div0", (void *)div0}, {0, 0}},
                                             {{"exit", (void *)exit}},
                                             {{"fontBig", (void *)fontBig}, {"fontSmall", (void *)fontSmall}},
                                             {{"getTime", (void *)getTime}, {0, 0}},
                                             {{"help", (void *)help}, {0, 0}},
                                             {{"invalidOpCode", (void *)invalidOpCode}, {0, 0}},
                                             {{0, 0}},
                                             {{"kill", (void *)kill}},
                                             {{"loop", (void *)loop}},
                                             {{"mem", (void *)memStatusPrinter}},
                                             {{"nice", (void *)nice}},
                                             {{0, 0}},
                                             {{"ps", (void *)listAllProcesses}},
                                             {{0, 0}},
                                             {{0, 0}},
                                             {{0, 0}},
                                             {{"testMem", (void *)createTestMem}, {"testMemInfo", (void *)createTestMemInfo}, {"testPrio", (void *)createTestPrio}, {"testProcesses", (void *)createTestProcesses}, {"testSync", (void *)createTestSync}}};

static char *commandNotFoundMsg = "Command %s not found. Type 'help' to see the list of commands\n";
// static uint8_t cNotFoundSize = 51;
static char *helpMsg = "PinguinOS - v.5.0\n\n"
                       "block: Block a process\n"
                       "clear: Clear the screen\n"
                       "div0: Divide by zero\n"
                       "exit: Exit the shell\n"
                       "fontBig: Increase the font size\n"
                       "fontSmall: Decrease the font size\n"
                       "getTime: Get the current time\n"
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
                       "cat: Reads from keyboard or from output of piped command and prints\n\n";
;
// static char *waitMsg = "Press any key to continue";
// ###################################################################

static uint16_t offsets[4096] = {0};
static char buffer[4096];
static uint16_t count;
static uint8_t exitFlag;
static uint16_t previousCount;
static uint16_t lineCount;
static uint16_t currentLine;
static uint16_t leftSteps;
static int IOPipes[2];
static uint64_t toWaitPID;

int launchShell(int argc, char *argv[])
{
    count = 0;
    buffer[count] = ' ';
    exitFlag = 0;
    previousCount = 0;
    currentLine = 1;
    leftSteps = 0;
    uint8_t key;
    IOPipes[0] = KEYBOARD_PIPE;
    IOPipes[1] = TERMINAL_PIPE;
    while (!exitFlag)
    {
        key = getChar();
        switch (key)
        {
        case UP_ARROW:
            // shellErrSound();
            //  sGetLastLine();
            break;
        case DOWN_ARROW:
            // shellErrSound();
            break;
        case RIGHT_ARROW:
            putChar(key);
            sMoveRight();
            break;
        case LEFT_ARROW:
            putChar(key);
            sMoveLeft();
            break;
        case DELETE:
            putChar(key);
            sDeleteChar();
            break;
        case '\n':
            putChar(key);
            sCheckCommand();
            previousCount = 0;
            offsets[lineCount++] = count;
            offsets[lineCount] = offsets[lineCount - 1]; // la linea esta vacia
            currentLine = lineCount;
            break;
        // case '\t':
        //     // if (offsets[lineCount] - offsets[lineCount - 1] == 1)
        //     // {
        //     //     char c = getCommandIdx(buffer[count - 1]);
        //     //     char *aux = commands[(unsigned char)c][0].name + 1;
        //     //     while (*aux)
        //     //     {
        //     //         sPrintChar(*aux);
        //     //         buffer[count++] = *aux;
        //     //         aux++;
        //     //         currentX++;
        //     //     }
        //     //     offsets[lineCount] = count;
        //     //     buffer[count] = ' ';
        //     //     sPrintSelected(' ');
        //     }
        case 0:
            break;
        default:
        {

            // for (uint16_t i = 0; i < leftSteps; i++)
            // {
            //     buffer[count - i] = buffer[count - i - 1];
            //     sPrintChar(buffer[count - i]);
            //     if (currentX == 0)
            //     {
            //         currentX = width - 1;
            //         currentY--;
            //     }
            //     else
            //         currentX--;
            // }
            // currentX = auxX;
            // currentY = auxY;
            buffer[count - leftSteps] = key;
            count++;
            offsets[lineCount] = count;
            buffer[count] = 0;
            putChar(key);
            // currentX++;
            // if (currentX == width)
            // {
            //     if (currentY < height - 1)
            //         currentY++;
            //     else
            //         sMoveScreenUp(1);
            //     currentX = 0;
            // }
            // sPrintSelected(buffer[count - leftSteps]);
        }
        }
    }
    return 0;
}

// void sGetLastLine()
// {
//     uint16_t currentOffset = offsets[lineCount] - offsets[lineCount - 1], l;
//     do
//     {
//         if (currentLine)
//             currentLine--;
//         else
//             return;
//         l = offsets[currentLine + 1] - offsets[currentLine];
//     } while (!l);
//     previousCount += l;
//     clearLineFrom(2, width);
//     count -= currentOffset;
//     currentX = 2;
//     for (uint16_t i = 0; i < l; i++)
//     {
//         if (currentX == width)
//         {
//             if (currentY < height - 1)
//                 currentY++;
//             else
//                 sMoveScreenUp(1);
//             currentX = 0;
//         }
//         uint8_t aux = buffer[count - previousCount];
//         buffer[count++] = aux;
//         sPrintChar(aux);
//         currentX++;
//     }
//     buffer[count] = ' ';
//     offsets[lineCount] = count;
//     sPrintSelected(' ');
// }

char getCommandIdx(char c)
{
    return c - 'a';
}

int sCheckToken(char *tokens[MAX_ARGS + 1], char *token)
{
    int i = 0;
    while (tokens[i] != NULL)
    {
        if (strcmp(tokens[i], token) == 0)
        {

            tokens[i] = NULL;
            return i;
        }
        i++;
    }
    return -1;
}

void sCheckCommand()
{
    if (offsets[lineCount] == offsets[lineCount - 1])
        return;
    uint8_t aux = buffer[offsets[lineCount]];

    char *command = &buffer[offsets[lineCount - 1]];
    char **command_tokens = getArgs(command);

    uint8_t j = 0;
    do
    {
        j++;
    } while (command_tokens[j] != NULL);
    j++;
    // uint8_t j = 0;
    // // do
    // // {
    // //     token = strtok(command, " ");
    // //     command_tokens[j++] = token;
    // //     command = NULL;
    // // } while (token);
    int pipePos = sCheckToken(command_tokens, "|");
    int ampersandPos = sCheckToken(command_tokens, "&");
    if (ampersandPos != -1)
    {
        j = j - 1;
    }
    if (pipePos != -1)
    {
        // printf("Piped command!!!\n");
        int connectionPipeId = createNewPipe();
        if (connectionPipeId == -1)
        {
            return;
        }
        IOPipes[1] = connectionPipeId;
        if (findAndExecCmd(command_tokens[0], pipePos - 1, command_tokens + 1, ampersandPos) == 1)
        {
            IOPipes[0] = connectionPipeId;
            IOPipes[1] = TERMINAL_PIPE;
            findAndExecCmd(command_tokens[pipePos + 1], j - pipePos - 3, command_tokens + pipePos + 2, ampersandPos);
            // printf("Piped command executed!!!\n");
            buffer[offsets[lineCount]] = aux;
        }
        IOPipes[0] = KEYBOARD_PIPE;
    }
    else
    {
        // printf("Normal command!!!\n");
        findAndExecCmd(command_tokens[0], j - 2, command_tokens + 1, ampersandPos);
        buffer[offsets[lineCount]] = aux;
    }
}

int findAndExecCmd(char *cmdName, int argc, char *argv[], int foreground)
{
    char c = getCommandIdx(cmdName[0]);
    if (c < 0 || c >= LETTERS)
    {
        printf(commandNotFoundMsg, cmdName);
        return 0;
    }

    buffer[offsets[lineCount]] = 0;
    command_t *auxC = commands[(unsigned char)c];
    for (int i = 0; i < WORDS; i++)
    {
        if (auxC[i].name != NULL)
        {
            int cmp = strcmp(cmdName, auxC[i].name);
            if (cmp < 0)
            {
                break;
            }
            else if (cmp == 0)
            {
                auxC[i].function(argc, argv);
                if (foreground == -1)
                {
                    setForegroundProcessCaller(UNUSED, toWaitPID);
                    waitPID(toWaitPID);
                }

                return 1;
            }
        }
        else
        {
            break;
        }
    }
    printf(commandNotFoundMsg, cmdName);
    return 0;
}

int createNewPipe()
{
    int created = -1;
    int i = 1;
    while (created == -1 && i < MAX_PIPES)
    {
        i++;
        created = newPipe(i);
    }
    if (created == -1)
    {
        printf("No more pipes available\n");
        return -1;
    }
    return i;
}

char **getArgs(char *buffer)
{
    static char *argv[10]; // Assuming a maximum of 10 arguments
    int argc = 0;
    char *aux = buffer;

    while (*aux != 0 && argc < 10)
    {
        while (*aux == ' ') // Skip leading spaces
            aux++;
        argv[argc++] = aux;
        while (*aux != ' ' && *aux != 0) // Find the end of the argument
            aux++;
        if (*aux != 0)
            *aux++ = 0; // Null-terminate the argument and move to the next character
    }
    argv[argc] = NULL; // Null-terminate the argument list
    return argv;
}

// comandos de la terminal

void fontBig(int argc, char *argv[])
{
    fontSizeUpCaller(UNUSED);
}

void exit(int argc, char *argv[])
{
    exitFlag = 1;
    // firstLineOnScreen = lineCount - 1;
    // currentX = 0;
    // currentY = 0;
}

void div0(int argc, char *argv[])
{
    // fontSize = 1;
    divZero();
}

void fontSmall(int argc, char *argv[])
{
    fontSizeDownCaller(UNUSED);
}

void clearCmd(int argc, char *argv[])
{
    clearTerminalCaller(UNUSED);
}

void getTime(int argc, char *argv[]) // Chequear
{
    char clock[20];
    getTimeCaller(UNUSED, (char *)clock);
    printf(clock);
}

void help(int argc, char *argv[])
{
    printf(helpMsg);
}

void invalidOpCode(int argc, char *argv[])
{
    opcode();
}

void createTestSync(int argc, char *argv[])
{
    int pid = createProcess("test_sync", test_sync, argc, argv, IOPipes);
    waitPID(pid);
}

void createTestMemInfo(int argc, char *argv[])
{
    int pid = createProcess("test_mem", test_mem, argc, argv, IOPipes);
    waitPID(pid);
}

void createTestProcesses(int argc, char *argv[])
{
    int pid = createProcess("test_processes", test_processes, argc, argv, IOPipes);
    waitPID(pid);
}

void createTestPrio(int argc, char *argv[])
{
    int pid = createProcess("test_prio", test_prio, argc, argv, IOPipes);
    waitPID(pid);
}

void createTestMem(int argc, char *argv[])
{
    int pid = createProcess("test_mm", test_mm, argc, argv, IOPipes);
    waitPID(pid);
}

void createTestWaitShell(int argc, char *argv[])
{
    int pid = createProcess("test_wait_shell", test_wait_shell, argc, argv, IOPipes);
    waitPID(pid);
}

int test_wait_shell(int argc, char *argv[])
{
    printf("            Waiting...\n");
    for (int i = 0; i < 1000000000; i++)
        ;
    printf("Done but gonna write some more\n");
    printf("            Done!\n");
    exitProc(0);
    return 0;
}

void listAllProcesses()
{
    processInformation *toPrint = listProcessesInfo();
    processInformation *toFree = toPrint;
    printf("| PID |Priority | State | S.Base           | S.Pointer        | Parent PID | Name\n");

    char stackHex[9];
    char stackPointerHex[9];

    while (toPrint->pid != UINT64_MAX)
    {
        uint64ToHexString((uint64_t)toPrint->stack, stackHex, sizeof(stackHex));
        uint64ToHexString((uint64_t)toPrint->stack_pointer, stackPointerHex, sizeof(stackPointerHex));

        printf("|  %d  |    %d    |   %d   |     %s     |     %s     |     %d      | %s\n",
               (int)toPrint->pid,
               toPrint->priority,
               toPrint->state,
               stackHex,
               stackPointerHex,
               (int)toPrint->parent_pid,
               toPrint->name);
        toPrint++;
    }
    freeM(toFree);
}

void loop(int argc, char *argv[])
{
    if (argc != 1)
    {
        printf("Usage: loop <seconds> %s %s\n", argv[0], argv[1]);
        return;
    }
    toWaitPID = createProcess("loopPrinter", loopPrinter, argc, argv, IOPipes);
    // waitPID(pid);
}

int loopPrinter(int argc, char *argv[])
{
    int seconds = atoi(argv[0]);
    while (1)
    {
        // printf("Second %d\n", seconds);
        printf("Hello, my PID is %d\n", getPID());
        waitCaller(UNUSED, seconds);
        // waitCaller(UNUSED, 10);
    }
    exitProc(0);
    return 0;
}

void kill(int argc, char *argv[])
{
    int pid = atoi(argv[0]);
    killProcess(pid);
}

void nice(int argc, char *argv[])
{
    int pid = atoi(argv[0]);
    int priority = atoi(argv[1]);
    changeProcessPriority(pid, priority);
}

void block(int argc, char *argv[])
{
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

void memStatusPrinter(uint64_t argc, char *argv[])
{
    MemStatus *memStatus = memStatusCaller(UNUSED);
    printf("Total memory: %d B\n", memStatus->total_mem);
    printf("              %d KB\n", memStatus->total_mem / (1024));
    printf("              %d MB\n", memStatus->total_mem / (1024 * 1024));
    printf("\n");
    printf("Free memory: %d B\n", memStatus->free_mem);
    printf("             %d KB\n", memStatus->free_mem / (1024));
    printf("             %d MB\n", memStatus->free_mem / (1024 * 1024));
    printf("\n");
    printf("Occupied memory: %d B\n", memStatus->occupied_mem);
    printf("                 %d KB\n", memStatus->occupied_mem / (1024));
    printf("                 %d MB\n", memStatus->occupied_mem / (1024 * 1024));

    freeM(memStatus);
}

void catCmd(int argc, char *argv[])
{
    int pid = createProcess("cat", catProc, argc, argv, IOPipes);
    waitPID(pid);
}

int catProc(int argc, char *argv[])
{
    char c = 0;
    while (c != '\t') // CHEQUEAR esto tiene que ser EOF, falta implementar Ctrl+D
    {
        c = getChar();
        putChar(c);
    }
    printf("CAT HAS FINISHED\n");
    exitProc(0);
    return 1;
}

void sMoveRight()
{
    if (leftSteps == 0)
    {
        // shellErrSound();
        return;
    }
    leftSteps--;
}

void sMoveLeft()
{
    if (offsets[lineCount - 1] == count - leftSteps)
    {
        // shellErrSound();
        return;
    }
    leftSteps++;
}

void sDeleteChar()
{
    if (offsets[lineCount - 1] == count - leftSteps)
        return;

    for (uint16_t i = 0; i <= leftSteps; i++)
    {
        buffer[count - leftSteps + i - 1] = buffer[count - leftSteps + i];
    }
    count--;

    offsets[lineCount]--;
}