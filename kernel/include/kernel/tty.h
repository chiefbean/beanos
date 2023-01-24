#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>


void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void kprint(const char* message);
void kprint_num(int num, int base);
void kerror(const char* message);
void clear_screen();
void scroll_screen();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void update_cursor(int x, int y);

#endif