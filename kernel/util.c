//Code in this file is adapted from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

void memory_copy(char* source, char* dest, int no_bytes) {
	for(int i = 0; i < no_bytes; i++) {
		*(dest + i) = *(source + i);
	}
}
