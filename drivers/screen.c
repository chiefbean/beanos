//Code in this file is adapted from wiki.osdev.org/Bare_Bones

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "screen.h"
#include "../kernel/string.h"
#include "../kernel/low_level.h"

static const size_t VGA_HEIGHT = 25;
static const size_t VGA_WIDTH = 80;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xb8000;
	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	enable_cursor(terminal_row, terminal_column);
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	if(c == '\n') {
		terminal_column = 0;
		if(++terminal_row == VGA_HEIGHT)
			scroll_screen();
	}
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if(++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if(++terminal_row == VGA_HEIGHT)
				scroll_screen();
		}
	}
}

void terminal_write(const char* data, size_t size) {
	for(size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
}

void kprint(const char* message) {
	terminal_write(message, strlen(message));
	update_cursor(terminal_row, terminal_column + 1);
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
	for(size_t i = 0; i < ((VGA_HEIGHT-1)*VGA_WIDTH); i++) {
		terminal_buffer[i] = vga_entry(terminal_buffer[i+VGA_WIDTH], terminal_color);
	}
	for(size_t i = ((VGA_HEIGHT-1)*VGA_WIDTH); i < VGA_HEIGHT*VGA_WIDTH; i++) {
		terminal_buffer[i] = vga_entry(' ', terminal_color);
	}
	terminal_row = VGA_HEIGHT-1;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void update_cursor(int x, int y) {
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
