#include "terminal.h"
#include "videoDriver.h"
#include <stdint.h>

void startNewLine();
void sPrintSelected(uint8_t c);
void sMoveRight();
void sMoveLeft();
void sDeleteChar();
void sPrintNewLine();
void sPrintChar(uint8_t c);
void sMoveScreenUp(uint8_t n);
void resetTerminal();
void clear();

static uint16_t currentY;
static uint16_t currentX;
static uint16_t width;
static uint16_t height;
static uint16_t lineCount;
static uint16_t firstLineOnScreen;
static uint16_t leftSteps;
static uint8_t fontSize;
static uint8_t reset;

static uint16_t offsets[MAX_BUFFER] = {0};
static char buffer[MAX_BUFFER];
static uint16_t count;

// // void heyTerminal()
// // {
// //     uint8_t c = 0;
// //     while (!c)
// //         read(1, &c, 1);
// //     switch (c)
// //     {
// //     case RIGHT_ARROW:
// //         sMoveRight();
// //         break;
// //     case LEFT_ARROW:
// //         sMoveLeft();
// //         break;
// //     case DELETE:
// //         sDeleteChar();
// //         break;
// //     case '\n':
// //         sPrintChar(buffer[count - leftSteps]);
// //         leftSteps = 0;
// //         sPrintNewLine();
// //         sPrintSelected(buffer[count]);
// //         offsets[lineCount++] = count;
// //         offsets[lineCount] = offsets[lineCount - 1]; // la linea esta vacia
// //         break;
// //     case 0:
// //         break;
// //     default:
// //     {
// //         uint16_t auxX = currentX, auxY = currentY;
// //         if (currentX + leftSteps >= width)
// //             currentY++;
// //         currentX = (currentX + leftSteps) % width;
// //         for (uint16_t i = 0; i < leftSteps; i++)
// //         {
// //             buffer[count - i] = buffer[count - i - 1];
// //             sPrintChar(buffer[count - i]);
// //             if (currentX == 0)
// //             {
// //                 currentX = width - 1;
// //                 currentY--;
// //             }
// //             else
// //                 currentX--;
// //         }
// //         currentX = auxX;
// //         currentY = auxY;
// //         buffer[count - leftSteps] = c;
// //         count++;
// //         offsets[lineCount] = count;
// //         buffer[count] = 0;
// //         sPrintChar(c);
// //         currentX++;
// //         if (currentX == width)
// //         {
// //             if (currentY < height - 1)
// //                 currentY++;
// //             else
// //                 sMoveScreenUp(1);
// //             currentX = 0;
// //         }
// //         sPrintSelected(buffer[count - leftSteps]);
// //     }
// //     }
// // }

// // void terminalInit() // TIENE QUE INICIALIZARSE CON EL STDIN EN TERMINAL_PIPE Y EL STDOUT NO IMPORTA
// // {
// //     newPipe(TERMINAL_PIPE);
// //     lineCount = 1;
// //     firstLineOnScreen = 0;
// //     leftSteps = 0;
// //     fontSize = 1;
// //     reset = 0;
// //     count = 0;
// //     buffer[count] = ' ';
// //     width = getWidth() / 8 / fontSize;
// //     height = getHeight() / 16 / fontSize;
// //     clear();
// //     currentX = 0;
// //     currentY = 0;

// //     startNewLine();
// //     sPrintSelected(' ');
// }

void terminalInit()
{
    newPipe(TERMINAL_PIPE);
    resetTerminal();
}

void terminalProcess() // TIENE QUE INICIALIZARSE CON EL STDIN EN TERMINAL_PIPE Y EL STDOUT NO IMPORTA
{
    while (1)
    { // VER CONDICION DE LOOP
        uint8_t c = 0;
        while (!c)
            read(getRunningInputPipe(), &c, 1);
        switch (c)
        {
        case RIGHT_ARROW:
            sMoveRight();
            break;
        case LEFT_ARROW:
            sMoveLeft();
            break;
        case DELETE:
            sDeleteChar();
            break;
        case '\n':
            sPrintChar(buffer[count - leftSteps]);
            leftSteps = 0;
            sPrintNewLine();
            sPrintSelected(buffer[count]);
            offsets[lineCount++] = count;
            offsets[lineCount] = offsets[lineCount - 1]; // la linea esta vacia
            break;
        case 0:
            break;
        case ETX:
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
            buffer[count - leftSteps] = c;
            count++;
            if (count == MAX_BUFFER)
            {
                resetTerminal();
            }
            else
            {
                offsets[lineCount] = count;
                buffer[count] = 0;
                sPrintChar(c);
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
    }
}

void resetTerminal()
{
    count = 0;
    lineCount = 1;
    firstLineOnScreen = 0;
    leftSteps = 0;
    fontSize = 1;
    reset = 0;
    buffer[count] = ' ';
    width = getWidth() / 8 / fontSize;
    height = getHeight() / 16 / fontSize;
    clear();
    currentX = 0;
    currentY = 0;

    startNewLine();
    sPrintSelected(' ');
}

void sPrintChar(uint8_t c)
{
    drawchar(c, currentX, currentY, WHITE, BLACK);
}

void sPrintSelected(uint8_t c)
{
    if (c == 0)
        drawchar(c, currentX, currentY, WHITE, WHITE);
    else
        drawchar(c, currentX, currentY, BLACK, WHITE);
}

void startNewLine()
{
    currentX = 0;
    drawchar('$', currentX, currentY, NEW_LINE_GREEN, BLACK);
    currentX++;
    drawchar('>', currentX, currentY, NEW_LINE_GREEN, BLACK);
    currentX++;
    drawchar(' ', currentX, currentY, BLACK, BLACK);
    currentX++;
}

void clearLineFrom(uint16_t x, uint16_t to)
{
    uint16_t auxX = currentX;
    currentX = x;
    while (currentX < width)
    {
        printRectangle(currentX * (8 * fontSize), currentY * (16 * fontSize), 8 * fontSize, 16 * fontSize, BLACK);
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

void clearCmd()
{
    clear();
    reset = 1;
    firstLineOnScreen = lineCount - 1;
    currentX = 0;
    currentY = 0;
    startNewLine();
    sPrintSelected(' ');
}

void sMoveScreenUp(uint8_t n)
{
    uint16_t y = currentY - n + 1, l = firstLineOnScreen + n;
    currentY = 0;

    moveScreenUp(n * 16);
    while (y != currentY)
    {
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

// void printMsgAndWait(const char *msg, uint8_t size)
// {
//     if (currentY < height - 2)
//         currentY++;
//     else
//         sMoveScreenUp(2 + size / width);
//     currentX = 0;
//     for (uint16_t i = 0; i < size; i++)
//     {
//         sPrintChar(msg[i]);
//         if (currentX == width - 1)
//         {
//             currentX = 0;
//             currentY++;
//         }
//         else
//             currentX++;
//     }
//     currentY++;
//     currentX = 0;
//     for (uint8_t j = 0; waitMsg[j]; j++)
//     {
//         sPrintChar(waitMsg[j]);
//         currentX++;
//     }
//     char c = 0;
//     while (!(c = getChar()))
//         ;
//     for (uint8_t i = 0; i < 2 + size / width; i++)
//     {
//         clearLineFrom(0, i);
//         currentY--;
//     }
// }
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
        // shellErrSound();
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
        // shellErrSound();
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

void printBufferFrom(uint16_t start, uint16_t end)
{
    for (int i = start; i < end; i++)
    {
        sPrintChar(buffer[i]);
        currentX++;
    }
}

int tFontBig()
{
    if (fontSize == 2)
    {
        return 0;
    }
    int toRet = fontSizeBigger();
    fontSize = 2;
    height /= 2;
    width /= 2;
    sMoveScreenUp((lineCount - firstLineOnScreen) / 2);
    reset = 1;
    return toRet;
}

int tFontSmall()
{
    if (fontSize == 1)
    {
        return 0;
    }
    int toRet = fontSizeSmaller();
    fontSize = 1;
    height *= 2;
    width *= 2;
    sMoveScreenUp(0);
    reset = 1;
    return toRet;
}