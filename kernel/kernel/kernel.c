#include <kernel/tty.h>
#include <kernel.h>
#include <stdio.h>
#include <sys/isr.h>
#include <sys/irq.h>
#include <sys/gdt.h>
#include <sys/timer.h>

void kernel_main() {
	terminal_initialize();
	printf("Kernel initialized.\n");
	// enable_paging();
	// printf("Paging enabled.\n");

	init_descriptor_tables();
	isr_install();
	printf("ISRs installed.\n");

	initPIC(0x20, 0x28);
	printf("PICs initialized.\n");

	// asm("int $10");
	// printf("interrupted\n");
	__asm__ __volatile__("sti");
	printf("interrupts enabled\n");

	__asm__ __volatile__("int $10");
	printf("interrupted\n");

	// irq_install();
	// printf("IRQs installed.\n");
	
	// init_timer(50);
	// printf("timer init\n");

	for(;;) {
   		asm("hlt");
 	}
}
