# Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

OBJ = ${C_SOURCES:.c=.o}

all: os-image

run: all
	qemu-system-i386 os-image

os-image: boot/boot.o kernel/kernel.o
	i686-elf-gcc -T boot/linker.ld -o $@ -ffreestanding -O2 -nostdlib $< -lgcc

%.o : %.c ${HEADERS}
	i686-elf-gcc -ffreestanding -c $< -o $@ -std=gnu99 -O2 -Wall -Wextra

%.o : %.s
	i686-elf-as $< -o $@

clean:
	rm -rf *.o os-image
	rm -rf kernel/*.o drivers/*.o
