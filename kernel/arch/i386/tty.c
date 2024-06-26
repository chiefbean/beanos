#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
#include <string.h>
#include <kernel/tty.h>
#include <sys/io.h>
 
#include "vga.h"
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC03FF000;
 
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static uint16_t cursor_pos;
 
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	cursor_pos = terminal_row * VGA_WIDTH + terminal_row;
	enable_cursor(cursor_pos, cursor_pos+1);
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) {
	unsigned char uc = c;

	if(uc == '\n') {
		terminal_column = 0;
		if(++terminal_row == VGA_HEIGHT)
			scroll_screen();
		update_cursor(terminal_row, terminal_column);
	}
	else {
		terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
		if(++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if(++terminal_row == VGA_HEIGHT) {
				scroll_screen();
			}
			update_cursor(terminal_row, terminal_column);
		}
		update_cursor(terminal_row, terminal_column);
	}
}
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void kprint(const char* message) {
	terminal_write(message, strlen(message));
}

void kprint_num(int num, int base) {
	char* tmp = "";
	kprint(itoa(num, tmp, base));
}

void kerror(const char* message) {
	uint8_t oldColor = terminal_color;
	terminal_setcolor(vga_entry_color(12,0));
	terminal_write("ERROR: ", 7);
	terminal_color = oldColor;
	terminal_write(message, strlen(message));
}

void clear_screen() {
	terminal_row = 0;
	terminal_column = 0;
	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void scroll_screen() {
	// for(size_t i = 0; i < ((VGA_HEIGHT-1)*VGA_WIDTH); i++) {
	// 	terminal_buffer[i] = vga_entry(terminal_buffer[i+VGA_WIDTH], terminal_color);
	// }
	// for(size_t i = ((VGA_HEIGHT-1)*VGA_WIDTH); i < VGA_HEIGHT*VGA_WIDTH; i++) {
	// 	terminal_buffer[i] = vga_entry(' ', terminal_color);
	// }
	// terminal_row = VGA_HEIGHT-1;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int x, int y) {
	cursor_pos = (x + 1) * VGA_WIDTH + y;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (cursor_pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((cursor_pos >> 8) & 0xFF));
}
