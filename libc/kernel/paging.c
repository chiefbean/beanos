#include <stdint.h>
#include <kernel.h>

extern void loadPageDirectory(uint32_t*);
extern void enablePaging();

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void enable_paging() {
	for(int i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
	}

	uint32_t first_page_table[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024; i++) {
		first_page_table[i] = (i * 0x1000) | 3;
	}

	page_directory[0] = ((unsigned int)first_page_table) | 3;

	loadPageDirectory(page_directory);
	enablePaging();
}
