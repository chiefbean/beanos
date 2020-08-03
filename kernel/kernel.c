//Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

#include "../drivers/screen.h"

void main() {
    terminal_initialize();
	char* vid = (char*) 0xb8000;
	*vid = 'X';
	print("Hello");
}
