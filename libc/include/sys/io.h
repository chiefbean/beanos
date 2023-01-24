//Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
#ifndef _IO_H
#define _IO_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short data);

#ifdef __cplusplus
}
#endif

#endif
