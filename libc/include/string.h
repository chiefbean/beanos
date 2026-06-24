#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char* data);
char* itoa(uint32_t value, char * str, int base);
int atoi(char* value);
char* substr(char* src, size_t start, size_t len, char* dst);
int strtok(char* string, char tok, char** dst);
int strcmp(char* a, char* b);

#ifdef __cplusplus
}
#endif

#endif