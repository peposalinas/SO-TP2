#include <stdint.h>
#include "./naiveConsole.h"

uint32_t GetUint();
uint32_t Gs(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
int endless_loop(int argc, char **argv);
int endless_loop_print(int argc, char **argv);
uint32_t GetUniform(uint32_t max);
uint64_t testMM(char *c);