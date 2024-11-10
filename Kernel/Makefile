include Makefile.inc

KERNEL=kernel.bin
KERNEL_ELF=$(KERNEL:.bin=.elf)

# Conditional source files based on memory manager type
ifeq ($(MM_TYPE),BUDDY)
    MM_SOURCES=buddyMemoryManager.c
    EXTRA_FLAGS=-DUSE_BUDDY_MM
else
    MM_SOURCES=MemoryManagerCDT.c
    EXTRA_FLAGS=-DUSE_CUSTOM_MM
endif

BASE_SOURCES=$(filter-out buddyMemoryManager.c MemoryManagerCDT.c, $(wildcard *.c))
SOURCES=$(BASE_SOURCES) $(MM_SOURCES)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

# Update GCCFLAGS to include the memory manager type
GCCFLAGS+=$(EXTRA_FLAGS)

all: $(KERNEL)

debug: all
		$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

docker-test:
	#!/bin/bash
	docker start TP2
	docker exec -it TP2 bash -c "cd /root/SO-TP2/Kernel && gcc test_util.c MemoryManagerCDT.c test_mm.c -o testeo"

clean:
	rm -rf asm/*.o *.o *.bin *.elf

.PHONY: all clean docker-test
