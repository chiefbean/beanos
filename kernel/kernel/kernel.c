#include <kernel/tty.h>
#include <kernel.h>
#include <stdio.h>
#include <sys/isr.h>
#include <sys/gdt.h>
#include <sys/timer.h>

void kernel_main() {
	terminal_initialize();
	printf("Kernel initialized.\n");
	enable_paging();
	printf("Paging enabled.\n");

	initPIC(0x20, 0x28);
	printf("PICs initialized.\n");

	init_descriptor_tables();
	isr_install();
	printf("ISRs installed.\n");
	asm("int $10");

	//init_timer(50);

	for(;;) {
   		asm("hlt");
 	}
}
