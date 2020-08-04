//Code in this file is copied from https://cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "screen.h"
#include "../kernel/string.h"

static const size_t VGA_HEIGHT = 25;
static const size_t VGA_WIDTH = 80;

int terminal_row;
int terminal_column;
uint8_t terminal_color;
unsigned short* terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
	terminal_buffer = (unsigned short*) 0xb8000;
	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if(++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if(++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size) {
	for(int i = 0; i < strlen(data); i++) {
		terminal_putchar(data[i]);
	}
}

void kprint(const char* message) {
	terminal_write(message, strlen(message));
}

void clear_screen() {
	terminal_row = 0;
	terminal_column = 0;
	for(int y = 0; y < VGA_WIDTH; y++) {
		for(int x = 0; x < VGA_HEIGHT; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
