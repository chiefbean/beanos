all: os-image

run: all
	qemu-system-i386 os-image

os-image: kernel.bin boot.bin
	cat $^ > $@

kernel.bin: kernel_entry.o kernel.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x1000 $^ --oformat binary

kernel.o: kernel.c
	gcc -m32 -ffreestanding -c $< -o $@ -fno-pic

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

clean:
	rm *.bin *.o
