#ifndef TERMINAL_H
#define TERMINAL_H

#include "definitions.h"
#include "syscalls.h"

#define WHITE 0x00FFFFFF
#define BLACK 0x00000000
#define NEW_LINE_GREEN 0x000BE675

#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254

void heyTerminal();
void terminalInit();
void tFontSmall();
void tFontBig();
void clear();

#endif