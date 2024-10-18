#include <stdint.h>
#include <kernel.h>

extern void loadPageDirectory(uint32_t*);
extern void enablePaging();

// uint32_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
// uint32_t page_dir[512] __attribute__((aligned(0x1000)));
uint32_t page_directory[1024] __attribute__((aligned(4096)));

void enable_paging() {

	// page_dir_ptr_tab[0] = (uint32_t)&page_dir | 1;
	// page_dir[0] = 0b10000011;

	// __asm__ __volatile__("movl %%cr4, %%eax; bts $5, %%eax; movl %%eax, %%cr4" ::: "eax"); // set bit5 in CR4 to enable PAE		 
	// __asm__ __volatile__("movl %0, %%cr3" :: "r" (page_dir_ptr_tab)); // load PDPT into CR3
	// __asm__ __volatile__("movl %%cr0, %%eax; orl $0x80000000, %%eax; movl %%eax, %%cr0;" ::: "eax");

	for(int i = 0; i < 1024; i++) {
		page_directory[i] = 0x00000002;
	}

	uint32_t first_page_table[1024] __attribute__((aligned(4096)));
	unsigned int i;
	//we will fill all 1024 entries in the table, mapping 4 megabytes
	for(i = 0; i < 1024; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}

	page_directory[0] = ((unsigned int)first_page_table) | 3;

	loadPageDirectory(page_directory);
	enablePaging();
}
