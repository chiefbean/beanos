#include <stddef.h>

size_t strlen(char* data) {
	size_t len = 0;
	while(data[len]) {
		len++;
	}
	return len;
}
