#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined (__is_libk)
#include <kernel/tty.h>
#endif

int putchar(int ic) {
#if defined (__is_libk)
    char c = (char) ic;
    terminal_write(&c, sizeof(c));
#else

#endif

    return ic;
}

int puts(const char* str) {
    return printf("%s\n", str);
}

static bool print(const char* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;

            if (maxrem < amount)
                return -1;
            if(!print(format, amount))
                return -1;

            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            if (!maxrem)
                return -1;

            if (!print(&c, sizeof(c)))
                return -1;

            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len)
                return -1;
            if (!print(str, len))
                return -1;

            written += len;
        } else if (*format == 'd' || *format == 'i') {
            format++;

            int num = va_arg(parameters, int);

            unsigned mag = (num < 0) ? -(unsigned)num : (unsigned)num;

            char buf[13];
            itoa(mag, buf, 10);

            size_t len = strlen(buf);
            if (maxrem < len)
                return -1;
            if (num < 0) {
                memmove(buf + 1, buf, len + 1);
                buf[0] = '-';
                if(!print(buf, len + 1))
                    return -1;
                written += len + 1;
            } else {
                if(!print(buf, len))
                    return -1;
                written += len;
            }

        } else if (*format == 'x') {
            format++;

            unsigned int num = va_arg(parameters, unsigned int);

            char buf[14];
            itoa(num, buf, 16);

            size_t len = strlen(buf) + 2;

            memmove(buf + 2, buf, len);
            buf[0] = '0';
            buf[1] = 'x';

            if (maxrem < len)
                return -1;
            if(!print(buf, len))
                return -1;

            written += len;

        } else if (*format == 'p') {
            format++;

            uintptr_t num = va_arg(parameters, uintptr_t);

            char buf[14];
            itoa(num, buf, 16);

            size_t len = strlen(buf) + 2;

            memmove(buf + 2, buf, len);
            buf[0] = '0';
            buf[1] = 'x';

            if (maxrem < len)
                return -1;
            if(!print(buf, len))
                return -1;

            written += len;

        } else if (*format == 'o') {
            format++;

            unsigned int num = va_arg(parameters, unsigned int);

            char buf[13];
            itoa(num, buf, 8);

            size_t len = strlen(buf);
            if (maxrem < len)
                return -1;
            if(!print(buf, len))
                return -1;

            written += len;

        } else if (*format == 'u') {
            format++;

            unsigned int num = va_arg(parameters, unsigned int);

            char buf[12];
            itoa(num, buf, 10);

            size_t len = strlen(buf);
            if (maxrem < len)
                return -1;
            if(!print(buf, len))
                return -1;

            written += len;

        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len)
                return -1;
            if(!print(format, len))
                return -1;

            written += len;
            format += len;
        }
    }
    va_end(parameters);
    return written;
}
