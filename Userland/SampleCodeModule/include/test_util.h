#include <stdint.h>

int test_mem(int argc, char *argv[]);
int test_sync(int argc, char *argv[]);
int test_processes(int argc, char *argv[]);
int test_prio(int argc, char *argv1[]);
int test_mm(int argc, char *argv[]);
uint32_t GetUint();
uint32_t Gs(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
int endless_loop(int argc, char *argv[]);
int endless_loop_print(int argc, char *argv[]);
uint32_t GetUniform(uint32_t max);
void bussy_wait(uint64_t n);
uint64_t testMM(char *c);