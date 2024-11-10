#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define MAX_PIPES 256
#define PIPE_SIZE 1024
#define FIRST_SEM_FOR_PIPES 1024
#define LAST_SEM_FOR_PIPES 1024 + MAX_PIPES * 2

#define KEYBOARD_PIPE 0
#define TERMINAL_PIPE 1

// ###################################################################

// keys, make and break codes

#define RIGHTKEY 0x4D
#define LEFTKEY 0x4B
#define UPKEY 0x48
#define DOWNKEY 0x50

#define DELETE 127
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254
#define FIRST_BREAK_CODE 128
#define ESC_MAKE_CODE 0x01
#define ESC_BREAK_CODE 0x81
#define R_MAKE_CODE 0x13
#define R_BREAK_CODE 0x93
#define D_MAKE_CODE 0x20
#define C_MAKE_CODE 0x2E
#define ENTER_B 0x9C
#define CAPS_LOCK 0x3A
#define ETX 0x03

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define LEFT_CTRL 0x1D

#define LEFT_SHIFT_B 0xAA
#define RIGHT_SHIFT_B 0xB6
#define LEFT_CTRL_B 0x9D

#endif