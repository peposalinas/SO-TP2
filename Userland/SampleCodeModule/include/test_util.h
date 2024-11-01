#include <stdint.h>

uint64_t test_mem(int argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]);
uint32_t GetUint();
uint32_t Gs(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);
uint32_t GetUniform(uint32_t max);
uint64_t testMM(char *c);