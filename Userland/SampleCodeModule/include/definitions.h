#ifndef _DEFINITIONS_USER_H_
#define _DEFINITIONS_USER_H_

#define MAX_PIPES 256
#define PIPE_SIZE 1024
#define FIRST_SEM_FOR_PIPES 1024
#define LAST_SEM_FOR_PIPES (1024 + MAX_PIPES * 2)

#define KEYBOARD_PIPE 0
#define TERMINAL_PIPE 1

#define BUFF_MAX 4096
#define MAX_ARGS 10

#define WHITE 0x00FFFFFF
#define BLACK 0

#define DELETE 127
#define EXIT 249
#define SHELL_NL 250
#define LEFT_ARROW 251
#define RIGHT_ARROW 252
#define UP_ARROW 253
#define DOWN_ARROW 254
#define ETX 0x03

#define LETTERS 'z' - 'a' + 1
#define WORDS 5

#endif