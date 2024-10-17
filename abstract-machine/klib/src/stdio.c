#ifndef TEST
#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#endif
#ifdef TEST
#define panic(str) do {} while(0)
#include "string.h"
#endif

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
void int_to_str(int num, char* str);


void int_to_str(int num, char* str) {
	int i = 0, is_negative = 0;

	// Handle the negative numbers
	if (num < 0) {
		is_negative = 1;
		num = -num;
	}

	// Convert the number to string
	do {
		str[i++] = (num % 10) + '0';
		num /= 10;
	} while (num > 0);

	// Add rhe negative sign if needed
	if (is_negative) {
		str[i++] = '-';
	}
	// Null-terminate the string
	str[i] = '\0';

	// Reverse the string
	for ( int j = 0, k = i - 1; j < k ; j++, k--) {
		char temp = str[j];
		str[j] = str[k];
		str[k] = temp;
	}
}
int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char *buf_ptr = out;
	const char *fmt_ptr = fmt;

	while (*fmt_ptr != '\0') {
		if (*fmt_ptr == '%') {
			fmt_ptr++; // Move to the format specifier

			if (*fmt_ptr == 'd') {
				int i = va_arg(args, int);
				char num_str[20];
				int_to_str(i, num_str);	// Convert integer to string
				strcpy(buf_ptr, num_str);	// Copy the number string to buffer
				buf_ptr += strlen(num_str);
			}
			else if (*fmt_ptr == 's') {
				char *s = va_arg(args, char *);
				strcpy(buf_ptr, s);
				buf_ptr += strlen(s);
			}
		} else {
			*buf_ptr++ = *fmt_ptr; // Copy other characters
		}
		fmt_ptr++;	// Move to the next character in format
	}
	*buf_ptr = '\0';	// Null-terminate the buffer
	va_end(args);

	return buf_ptr - out;	// Return the length of the string
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
