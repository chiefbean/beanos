#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char* data);
char* itoa(int value, char * str, int base);

#ifdef __cplusplus
}
#endif

#endif