#include <sys/io.h>

unsigned char inb(unsigned short port) {
    //reads a byte in from the specified port
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}