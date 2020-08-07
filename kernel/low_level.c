//Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

#include "low_level.h"

unsigned char inb(unsigned short port) {
    //reads a byte in from the specified port
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void outb(unsigned short port, unsigned char data) {
    //writes data to port
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void outw(unsigned short port, unsigned short data) {
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}
