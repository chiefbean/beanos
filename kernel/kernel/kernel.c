#include <kernel/tty.h>
#include <kernel.h>
#include <stdio.h>
#include <sys/isr.h>

void kernel_main() {
	terminal_initialize();
	printf("Kernel initialized.\n");
	enable_paging();
	printf("Paging enabled.\n");
	
	isr_install();
	printf("ISRs installed.\n");

	init_pics(0x20, 0x28);
	printf("PICs initialized.\n");

	for(;;) {
   		asm("hlt");
 	}
}
