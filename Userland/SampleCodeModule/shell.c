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

void test_wait_shell(int argc, char *argv[]);
void createTestWaitShell(int argc, char *argv[]);

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
static void doNothing();

static command_t commands[LETTERS][WORDS] = {{{0, 0}}, {{0, 0}}, {{"clear", clearCmd}, {0, 0}}, {{"div0", div0}, {0, 0}}, {{"exit", exit}}, {{"fontBig", fontBig}, {"fontSmall", fontSmall}}, {{"getTime", getTime}, {0, 0}}, {{"help", help}, {0, 0}}, {{"invalidOpCode", invalidOpCode}, {0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{"ps", listAllProcesses}}, {{0, 0}}, {{0, 0}}, {{0, 0}}, {{"testMem", createTestMem}, {"testMemInfo", createTestMemInfo}, {"testPrio", createTestPrio}, {"testProcesses", createTestProcesses}, {"testSync", createTestSync}}};

static char *commandNotFoundMsg = "Command not found. Type 'help' to see the list of commands";
static uint8_t cNotFoundSize = 51;
static char *helpMsg = "PinguinOS - v.5.0\n\n"
                       "clear: Clear the screen\n"
                       "div0: Divide by zero\n"
                       "exit: Exit the shell\n"
                       "fontBig: Increase the font size\n"
                       "fontSmall: Decrease the font size\n"
                       "getTime: Get the current time\n"
                       "help: Show this message\n"
                       "invalidOpCode: Execute an invalid operation code\n"
                       "ps: List all processes\n"
                       "testMemInfo: Test memory info\n"
                       "testSync: Test sync\n";
static uint8_t hMsgSize = 97;
static char *waitMsg = "Press any key to continue";
// ###################################################################

static uint16_t currentY;
static uint16_t currentX;
static uint16_t width;
static uint16_t height;
static uint8_t buffer[4096];
static uint16_t count;
static uint16_t offsets[4096] = {0};
static uint16_t lineCount;
static uint16_t firstLineOnScreen;
static uint16_t leftSteps;
static uint16_t currentLine;
static uint16_t previousCount;
static uint8_t exitFlag;
static uint8_t fontSize;
static uint8_t reset;

void sPrintChar(uint8_t c)
{
    printCharCaller(UNUSED, c, currentX, currentY, WHITE, BLACK);
}

void sPrintSelected(uint8_t c)
{
    if (c == 0)
        printCharCaller(UNUSED, c, currentX, currentY, WHITE, WHITE);
    else
        printCharCaller(UNUSED, c, currentX, currentY, BLACK, WHITE);
}

void startNewLine()
{
    currentX = 0;
    sPrintChar('$');
    currentX++;
    sPrintChar('>');
    currentX++;
}

void clearLineFrom(uint16_t x, uint16_t to)
{
    uint16_t auxX = currentX;
    currentX = x;
    while (currentX < width)
    {
        printRectangleCaller(UNUSED, currentX * (8 * fontSize), currentY * (16 * fontSize), 8 * fontSize, 16 * fontSize, BLACK);
        currentX++;
    }
    currentX = auxX;
}
void clear()
{
    uint16_t auxX = currentX, auxY = currentY;
    currentX = 0;
    currentY = 0;
    while (currentY < height)
    {
        clearLineFrom(0, width);
        currentY++;
    }
    currentX = auxX;
    currentY = auxY;
}

void sMoveScreenUp(uint8_t n)
{
    clear();
    uint16_t y = currentY - n + 1, l = firstLineOnScreen + n;
    currentY = 0;

    while (y != currentY)
    {
        startNewLine();
        for (uint16_t i = 0; i < offsets[l + 1] - offsets[l]; i++)
        {
            if (currentX == width)
            {
                currentY++;
                currentX = 0;
            }
            sPrintChar(buffer[offsets[l] + i]);

            currentX++;
        }
        l++;
        currentY++;
    }

    firstLineOnScreen += n;
}

void sPrintNewLine()
{
    if (!reset)
    {
        if (currentY < height - 1)
            currentY++;
        else
            sMoveScreenUp(1);
    }
    startNewLine();
    reset = 0;
}

void printMsgAndWait(const char *msg, uint8_t size)
{
    if (currentY < height - 2)
        currentY++;
    else
        sMoveScreenUp(2 + size / width);
    currentX = 0;
    for (uint16_t i = 0; i < size; i++)
    {
        sPrintChar(msg[i]);
        if (currentX == width - 1)
        {
            currentX = 0;
            currentY++;
        }
        else
            currentX++;
    }
    currentY++;
    currentX = 0;
    for (uint8_t j = 0; waitMsg[j]; j++)
    {
        sPrintChar(waitMsg[j]);
        currentX++;
    }
    char c = 0;
    while (!(c = getChar()))
        ;
    for (uint8_t i = 0; i < 2 + size / width; i++)
    {
        clearLineFrom(0, i);
        currentY--;
    }
}

char getCommandIdx(char c)
{
    return c - 'a';
}

void sCheckCommand()
{
    if (offsets[lineCount] == offsets[lineCount - 1])
        return;
    uint8_t aux = buffer[offsets[lineCount]];

    char *command_tokens[MAX_ARGS + 1];
    char *token;
    char *command = &buffer[offsets[lineCount - 1]];
    uint8_t j = 0;
    do
    {
        token = strtok(command, " ");
        command_tokens[j++] = token;
        command = NULL;
    } while (token);

    char c = getCommandIdx(command_tokens[0][0]);
    if (c < 0 || c >= LETTERS)
    {
        printMsgAndWait(commandNotFoundMsg, cNotFoundSize);
        return;
    }

    buffer[offsets[lineCount]] = 0;
    command_t *auxC = commands[c];
    for (int i = 0; i < WORDS; i++)
    {
        if (auxC[i].name != NULL)
        {
            int cmp = strcmp(buffer + offsets[lineCount - 1], auxC[i].name);
            if (cmp < 0)
            {
                break;
            }
            else if (cmp == 0)
            {
                auxC[i].function(j - 2, command_tokens + 1);
                buffer[offsets[lineCount]] = aux;
                return;
            }
        }
        else
        {
            break;
        }
    }
    printMsgAndWait(commandNotFoundMsg, cNotFoundSize);
    buffer[offsets[lineCount]] = aux;
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

void sDeleteChar()
{
    if (offsets[lineCount - 1] == count - leftSteps)
        return;
    if (currentX == 0)
    {
        currentY--;
        currentX = width - 1;
    }
    else
        currentX--;
    uint16_t auxX = currentX, auxY = currentY;
    for (uint16_t i = 0; i <= leftSteps; i++)
    {
        buffer[count - leftSteps + i - 1] = buffer[count - leftSteps + i];
        sPrintChar(buffer[count - leftSteps + i]);
        if (currentX == width - 1)
        {
            currentX = 0;
            currentY++;
        }
        else
            currentX++;
    }
    sPrintChar(buffer[count]);
    currentX = auxX;
    currentY = auxY;
    sPrintSelected(buffer[count - leftSteps - 1]);
    count--;

    offsets[lineCount]--;
}

void sMoveLeft()
{
    if (offsets[lineCount - 1] == count - leftSteps)
    {
        shellErrSound();
        return;
    }
    sPrintChar(buffer[count - leftSteps]);
    if (currentX == 0)
    {
        currentX = width - 1;
        currentY--;
    }
    else
        currentX--;
    leftSteps++;
    sPrintSelected(buffer[count - leftSteps]);
}

void sMoveRight()
{
    if (leftSteps == 0)
    {
        shellErrSound();
        return;
    }
    sPrintChar(buffer[count - leftSteps]);
    if (currentX == width)
    {
        currentX = 0;
        currentY++;
    }
    else
        currentX++;
    leftSteps--;
    sPrintSelected(buffer[count - leftSteps]);
}

void sGetLastLine()
{
    uint16_t currentOffset = offsets[lineCount] - offsets[lineCount - 1], l;
    do
    {
        if (currentLine)
            currentLine--;
        else
            return;
        l = offsets[currentLine + 1] - offsets[currentLine];
    } while (!l);
    previousCount += l;
    clearLineFrom(2, width);
    count -= currentOffset;
    currentX = 2;
    for (uint16_t i = 0; i < l; i++)
    {
        if (currentX == width)
        {
            if (currentY < height - 1)
                currentY++;
            else
                sMoveScreenUp(1);
            currentX = 0;
        }
        uint8_t aux = buffer[count - previousCount];
        buffer[count++] = aux;
        sPrintChar(aux);
        currentX++;
    }
    buffer[count] = ' ';
    offsets[lineCount] = count;
    sPrintSelected(' ');
}

void printBufferFrom(uint16_t start, uint16_t end)
{
    for (int i = start; i < end; i++)
    {
        sPrintChar(buffer[i]);
        currentX++;
    }
}

void launchShell()
{
    count = 0;
    lineCount = 1;
    firstLineOnScreen = 0;
    leftSteps = 0;
    currentLine = 1;
    previousCount = 0;
    fontSize = 1;
    reset = 0;
    width = getScreenWidthCaller(UNUSED) / 8 / fontSize;
    height = getScreenHeightCaller(UNUSED) / 16 / fontSize;
    clear();
    currentX = 0;
    currentY = 0;
    exitFlag = 0;
    startNewLine();
    sPrintSelected(' ');
    buffer[count] = ' ';

    // Debuggeo
    char *argvTest[2] = {"10", NULL};
    int debugging_pid = createProcess("test_mem_info", 4, test_mem, 1, argvTest);
    waitPID(debugging_pid);
    // Final debuggeo

    uint8_t key;
    while (!exitFlag)
    {
        key = getChar();
        switch (key)
        {
        case RIGHT_ARROW:
            sMoveRight();
            break;
        case LEFT_ARROW:
            sMoveLeft();
            break;
        case UP_ARROW:
            sGetLastLine();
            break;
        case DOWN_ARROW:
            shellErrSound();
            break;
        case DELETE:
            sDeleteChar();
            break;
        case '\n':
            sPrintChar(buffer[count - leftSteps]);
            sCheckCommand();
            leftSteps = 0;
            previousCount = 0;
            sPrintNewLine();
            sPrintSelected(buffer[count]);
            offsets[lineCount++] = count;
            offsets[lineCount] = offsets[lineCount - 1]; // la linea esta vacia
            currentLine = lineCount;
            break;
        case '\t':
            if (offsets[lineCount] - offsets[lineCount - 1] == 1)
            {
                char c = getCommandIdx(buffer[count - 1]);
                char *aux = commands[c][0].name + 1;
                while (*aux)
                {
                    sPrintChar(*aux);
                    buffer[count++] = *aux;
                    aux++;
                    currentX++;
                }
                offsets[lineCount] = count;
                buffer[count] = ' ';
                sPrintSelected(' ');
            }
        case 0:
            break;
        default:
        {
            uint16_t auxX = currentX, auxY = currentY;
            if (currentX + leftSteps >= width)
                currentY++;
            currentX = (currentX + leftSteps) % width;
            for (uint16_t i = 0; i < leftSteps; i++)
            {
                buffer[count - i] = buffer[count - i - 1];
                sPrintChar(buffer[count - i]);
                if (currentX == 0)
                {
                    currentX = width - 1;
                    currentY--;
                }
                else
                    currentX--;
            }
            currentX = auxX;
            currentY = auxY;
            buffer[count - leftSteps] = key;
            count++;
            offsets[lineCount] = count;
            buffer[count] = 0;
            sPrintChar(key);
            currentX++;
            if (currentX == width)
            {
                if (currentY < height - 1)
                    currentY++;
                else
                    sMoveScreenUp(1);
                currentX = 0;
            }
            sPrintSelected(buffer[count - leftSteps]);
        }
        }
    }
    while (fontSizeDownCaller(UNUSED))
        fontSize--;
}

// comandos de la terminal

void fontBig(int argc, char *argv[])
{
    if (fontSize == 2)
    {
        shellErrSound();
        return;
    }
    fontSizeUpCaller(UNUSED);
    fontSize = 2;
    height /= 2;
    width /= 2;
    sMoveScreenUp((currentLine - firstLineOnScreen) / 2);
    reset = 1;
}

void exit(int argc, char *argv[])
{
    exitFlag = 1;
    firstLineOnScreen = lineCount - 1;
    currentX = 0;
    currentY = 0;
}

void div0(int argc, char *argv[])
{
    fontSize = 1;
    divZero();
}

void fontSmall(int argc, char *argv[])
{
    if (fontSize == 1)
    {
        shellErrSound();
        return;
    }
    fontSizeDownCaller(UNUSED);
    fontSize = 1;
    height *= 2;
    width *= 2;
    sMoveScreenUp(0);
    reset = 1;
}

void clearCmd(int argc, char *argv[])
{
    clear();
    reset = 1;
    firstLineOnScreen = lineCount - 1;
    currentX = 0;
    currentY = 0;
}

void getTime(int argc, char *argv[])
{
    uint8_t clock[20];
    getTimeCaller(UNUSED, clock);
    printMsgAndWait(clock, 8);
}

void help(int argc, char *argv[])
{
    printf(helpMsg);
    // printMsgAndWait(helpMsg, hMsgSize);
}

void invalidOpCode(int argc, char *argv[])
{
    fontSize = 1;
    opcode();
}

void createTestSync(int argc, char *argv[])
{
    int pid = createProcess("test_sync", 4, test_sync, argc, argv);
    waitPID(pid);
}

void createTestMemInfo(int argc, char *argv[])
{
    int pid = createProcess("test_mem", 4, test_mem, argc, argv);
    waitPID(pid);
}

void createTestProcesses(int argc, char *argv[])
{
    int pid = createProcess("test_processes", 4, test_processes, argc, argv);
    waitPID(pid);
}

void createTestPrio(int argc, char *argv[])
{
    int pid = createProcess("test_prio", 4, test_prio, argc, argv);
    waitPID(pid);
}

void createTestMem(int argc, char *argv[])
{
    int pid = createProcess("test_mm", 4, test_mm, argc, argv);
    waitPID(pid);
}

void createTestWaitShell(int argc, char *argv[])
{
    int pid = createProcess("test_wait_shell", 4, test_wait_shell, argc, argv);
    waitPID(pid);
}

void test_wait_shell(int argc, char *argv[])
{
    printf("            Waiting...\n");
    for (int i = 0; i < 1000000000; i++)
        ;
    printf("Done but gonna write some more\n");
    printf("            Done!\n");
    exitProc(0);
}
void listAllProcesses()
{
    char *toPrint = listProcessesInfo();
    printf(toPrint);
    freeMCaller(UNUSED, toPrint);
}
