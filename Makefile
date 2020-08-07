# Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
CC=i686-elf-gcc
AS=i686-elf-as
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

CFLAGS=-ffreestanding -O2
LDFLAGS=$(CFLAGS) -nostdlib -lgcc
COMFLAGS=$(CFLAGS) -Wall -Wextra -std=gnu99

CRTI=boot/crti.o
CRTBEGIN:=$(shell $(CC) $(COMFLAGS) -print-file-name=crtbegin.o)
CRTEND:=$(shell $(CC) $(COMFLAGS) -print-file-name=crtend.o)
CRTN=boot/crtn.o

OBJ=${C_SOURCES:.c=.o}

all: os-image

run: all
	qemu-system-i386 -kernel os-image

os-image: $(CRTI) $(CRTBEGIN) boot/boot.o kernel/kernel.o ${OBJ} $(CRTEND) $(CRTN)
	$(CC) $(LDFLAGS) -T boot/linker.ld -o $@  $^

%.o : %.c ${HEADERS}
	$(CC) $(COMFLAGS) -c $< -o $@

%.o : %.s 
	$(AS) $< -o $@

clean:
	rm -rf *.o os-image
	rm -rf kernel/*.o drivers/*.o boot/*.o
