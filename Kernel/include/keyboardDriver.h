#ifndef _KEYBOARD_DRIVER_H
#define _KEYBOARD_DRIVER_H
#include <stdint.h>
#include "lib.h"
#include "regsDump.h"
#include "pipes.h"
#include "syscalls.h"
#include "scheduler.h"
#include "definitions.h"

void keyboardInit();

uint8_t keyHandler();

void keyboard_handler();

uint8_t nextFromBuffer();
uint8_t getKey(); // pulling
uint8_t getKeyboardStatus();
uint8_t getKeyboardKey(); // pullingn't

#endif