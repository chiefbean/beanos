#include "../drivers/screen.h"
#include "string.h"

void kernel_main() {
    terminal_initialize();
	kprint("Hello\n");
}
