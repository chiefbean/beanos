#ifndef _KEYBOARD_H
#define _KEYBOARD_H 1

#include <sys/io.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char buf[256];
    uint8_t head;
    uint8_t tail;
} key_buffer_t;

static key_buffer_t keybuffer;

char scancode_table[256];
char shift_scancode_table[256];

void init_keyboard();
char keyboard_getchar();
char* getline();
void init_scancodes();

#ifdef __cplusplus
}
#endif

#endif