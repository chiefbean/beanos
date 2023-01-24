#include <sys/io.h>

void outb(unsigned short port, unsigned char data) {
    //writes data to port
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}