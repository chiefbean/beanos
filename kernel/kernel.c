//Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

#include "../drivers/screen.h"
#include "string.h"

void main() {
    terminal_initialize();
	//terminal_write("Hello", 5);
	//terminal_putchar('A');
	//terminal_putentryat('c', 0x0f, 0,0);
	kprint("Hello");
	terminal_putchar('A');
}
