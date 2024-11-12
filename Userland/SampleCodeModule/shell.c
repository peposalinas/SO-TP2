// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>

// ###################################################################
// definiciones para los commandos
typedef struct command_t
{
    char *name;
    void (*function)(uint64_t argc, char *argv[]);
} command_t;

static void sMoveRight();
static void sMoveLeft();
static void sDeleteChar();
static void sCheckCommand();
static int findAndExecCmd(char *cmdName, int argc, char *argv[], int foreground);
static int createNewPipe();
static char **getArgs(char *buffer);
static int sCheckToken(char *tokens[MAX_ARGS + 1], char *token);
static void resetShell();
void exit(int argc, char *argv[]);

char *commandNotFoundMsg = "Command '%s' not found. Type 'help' to see the list of commands\n";

static command_t commands[LETTERS][WORDS] = {{{0, 0}},
                                             {{"block", (void *)block}},
                                             {{"cat", (void *)catCmd}, {"clear", (void *)clearCmd}, {0, 0}},
                                             {{"div0", (void *)div0}, {0, 0}},
                                             {{"exit", (void *)exit}},
                                             {{"filter", (void *)filter}, {"fontBig", (void *)fontBig}, {"fontSmall", (void *)fontSmall}},
                                             {{0, 0}},
                                             {{"help", (void *)help}, {0, 0}},
                                             {{"invalidOpCode", (void *)invalidOpCode}, {0, 0}},
                                             {{0, 0}},
                                             {{"kill", (void *)kill}},
                                             {{"loop", (void *)loop}},
                                             {{"mem", (void *)memStatusPrinter}},
                                             {{"nice", (void *)nice}},
                                             {{0, 0}},
                                             {{"philo", (void *)philo}, {"ps", (void *)listAllProcesses}},
                                             {{0, 0}},
                                             {{0, 0}},
                                             {{0, 0}},
                                             {{"testMem", (void *)createTestMem}, {"testMemInfo", (void *)createTestMemInfo}, {"testPrio", (void *)createTestPrio}, {"testProcesses", (void *)createTestProcesses}, {"testSync", (void *)createTestSync}},
                                             {{0, 0}},
                                             {{0, 0}},
                                             {{"wc", (void *)wc}}};

// ###################################################################

static uint16_t offsets[BUFF_MAX] = {0};
static char buffer[BUFF_MAX];
static uint16_t count;
static uint8_t exitFlag;
static uint16_t previousCount;
static uint16_t lineCount;
static uint16_t currentLine;
static uint16_t leftSteps;

void resetShell()
{
    count = 0;
    buffer[count] = ' ';
    exitFlag = 0;
    previousCount = 0;
    currentLine = 1;
    leftSteps = 0;
}

int launchShell(int argc, char *argv[])
{
    resetShell();
    uint8_t key;
    setIOPipes(KEYBOARD_PIPE, TERMINAL_PIPE);
    setWaitPID(-1);
    putChar(SHELL_NL);
    while (!exitFlag)
    {
        key = getChar();
        switch (key)
        {
        case UP_ARROW:
            break;
        case DOWN_ARROW:
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
            putChar(SHELL_NL);
            previousCount = 0;
            offsets[lineCount++] = count;
            offsets[lineCount] = offsets[lineCount - 1];
            currentLine = lineCount;
            break;
        case 0:
            break;
        case ETX:
            break;
        default:
        {
            buffer[count - leftSteps] = key;
            count++;
            if (count == BUFF_MAX)
            {
                resetShell();
                printf("Buffer full, shell was restarted\n");
            }
            else
            {
                offsets[lineCount] = count;
                buffer[count] = 0;
                putChar(key);
            }
        }
        }
    }
    exitProc(0);
    return 0;
}

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
    int pipePos = sCheckToken(command_tokens, "|");
    int ampersandPos = sCheckToken(command_tokens, "&");
    if (ampersandPos != -1) // CHEQUEAR
    {
        j--;
    }
    if (pipePos != -1)
    {
        int connectionPipeId = createNewPipe();
        if (connectionPipeId == -1)
        {
            return;
        }
        setIOPipes(KEYBOARD_PIPE, connectionPipeId);
        if (findAndExecCmd(command_tokens[0], pipePos - 1, command_tokens + 1, ampersandPos) == 1)
        {
            setIOPipes(connectionPipeId, TERMINAL_PIPE);

            findAndExecCmd(command_tokens[pipePos + 1], j - pipePos - 3, command_tokens + pipePos + 2, ampersandPos);
            buffer[offsets[lineCount]] = aux;
        }
        setIOPipes(KEYBOARD_PIPE, TERMINAL_PIPE);
    }
    else
    {
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
                if (foreground != -1)
                {
                    setIOPipes(-1, TERMINAL_PIPE);
                }
                auxC[i].function(argc, argv);
                if (foreground == -1)
                {
                    setForegroundProcessCaller(UNUSED, getWaitPID());
                    waitPID(getWaitPID());
                }
                else
                {
                    setIOPipes(KEYBOARD_PIPE, TERMINAL_PIPE);
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
    static char *argv[10];
    int argc = 0;
    char *aux = buffer;

    while (*aux != 0 && argc < 10)
    {
        while (*aux == ' ')
            aux++;
        argv[argc++] = aux;
        while (*aux != ' ' && *aux != 0)
            aux++;
        if (*aux != 0)
            *aux++ = 0;
    }
    argv[argc] = NULL;
    return argv;
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

void exit(int argc, char *argv[])
{
    exitFlag = 1;
    clearTerminalCaller(UNUSED);
    fontSizeUpCaller(UNUSED);
    printf("\n\n        Goodbye! We hope you enjoyed PinguinOS - v.5.0");
    waitCaller(UNUSED, 2);
    putChar(EXIT);
}
