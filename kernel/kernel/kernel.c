#include <kernel/tty.h>
#include <kernel.h>
#include <stdio.h>
#include <sys/isr.h>
#include <sys/irq.h>
#include <sys/gdt.h>
#include <sys/timer.h>
#include <sys/keyboard.h>

void kernel_main() {
	terminal_initialize();
	printf("Kernel initialized.\n");

	init_descriptor_tables();
	isr_install();
	printf("ISRs installed.\n");

	initPIC(0x20, 0x28);
	printf("PICs initialized.\n");

	__asm__ __volatile__("sti");
	printf("interrupts enabled\n");

	irq_install();
	printf("IRQs installed.\n");
	
	init_timer(50);
	printf("timer init\n");

	init_keyboard();

	for(;;) {
   		char key = keyboard_getchar();
		if (key != '\0') printf("%c", key);
 	}
}
