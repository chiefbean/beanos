#include <sys/io.h>

void outw(unsigned short port, unsigned short data) {
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}