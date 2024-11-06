
all:  bootloader kernel userland image

bootloader:
	cd Bootloader; make all

buddy: export MM_TYPE=BUDDY
buddy: clean kernel userland image

myMM: export MM_TYPE=CUSTOM
myMM: clean kernel userland image

debug: all
	cd Kernel; make debug
	cd Userland; make debug

debug_buddy: export MM_TYPE=BUDDY
debug_buddy: clean kernel userland image
	cd Kernel; make debug
	cd Userland; make debug

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean buddy myMM debug debug_buddy