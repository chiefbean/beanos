#include <kernel.h>
#include <kernel/tty.h>
#include <kernel/shell.h>
#include <kernel/boot.h>
#include <stdio.h>
#include <sys/isr.h>
#include <sys/irq.h>
#include <sys/gdt.h>
#include <sys/timer.h>
#include <sys/keyboard.h>

void kernel_main(uint32_t magic, void* mbi) {
	terminal_initialize();
	printf("Kernel initialized.");

	boot_info_parse(magic, mbi);
	printf("\nParsed boot info.");

	init_descriptor_tables();
	isr_install();
	printf("\nISRs installed.");

	initPIC(0x20, 0x28);
	printf("\nPICs initialized.");

	__asm__ __volatile__("sti");
	printf("\nInterrupts enabled.");

	irq_install();
	printf("\nIRQs installed.");
	
	init_timer(50);
	printf("\nTimer init.");

	init_keyboard();
	printf("\n> ");

	for(;;) {
		char key = keyboard_getchar();
		if (key == '\0') continue;
		if (key == '\n') {
			char* line = getline();
			// do something with line
			shell_run(line);
			printf("\n> ");
		} else {
			printf("%c", key);
		}
	}
}
