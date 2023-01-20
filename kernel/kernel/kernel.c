#include <kernel/tty.h>
#include <pic.h>

void kernel_main() {
	enable_paging();
	terminal_initialize();
	terminal_writestring("Kernel initialized.\n");
	
	init_pics(0x20, 0x28);
	terminal_writestring("PICs initialized.\n");
}
