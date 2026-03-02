#include <string.h>
#include <stdint.h>

int memcmp(const void* aptr, const void* bptr, size_t size) {
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

void *memset(void *dest, int value, size_t count) {
	uint8_t val = (uint8_t)(value & 0xFF);
	uint8_t *dest2 = (uint8_t*)(dest);
	for (size_t i = 0; i < count; i++)
		dest2[i] = val;
	return dest;
}

size_t strlen(const char* data) {
	size_t len = 0;
	while(data[len])
		len++;
	return len;
}

char* itoa(uint32_t value, char * str, int base) {
	char * rc;
	char * ptr;
	char * low;

	if(base < 2 || base > 36) {
		return str;
	}

	rc = ptr = str;
	if(value < 0 && base == 10) {
		*ptr++ = '-';
	}

	low = ptr;

	do {
		*ptr++ = "zxywvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
		value /= base;
	} while(value);
	*ptr-- = '\0';

	while(low < ptr) {
		char tmp = *low;
		*low++ = *ptr;
		*ptr-- = tmp;
	}
	return rc;
}
