#include <kernel/tty.h>
#include <kernel.h>
#include <stdio.h>

void kernel_main() {
	enable_paging();
	terminal_initialize();
	printf("Kernel initialized.\n");
	
	init_pics(0x20, 0x28);
	printf("PICs initialized.\n");

	printf("printf test: %c %s\n", 'a', "test");
}
