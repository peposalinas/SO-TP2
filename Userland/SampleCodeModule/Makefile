include ../Makefile.inc

# @TODO: ver si hay mejor forma de compilar también los archivos dentro de "tests" (todos los .c dentro de subcarpetas) 

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=$(MODULE:.bin=.elf)

SOURCES=$(wildcard [^_]*.c tests/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=_loader.c
LOADEROBJECT=$(LOADERSRC:.c=.o)

all: $(MODULE)

debug: all
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../$(MODULE_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)

$(MODULE): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS)  -I./include -c $< -o $@
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o tests/*.o

.PHONY: all clean print
