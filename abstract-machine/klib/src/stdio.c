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

#define MAX_STRING_LEN 1024
void int_to_str(int num, char* str);
int process_format_string(char *out, const char *fmt, va_list args);

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


int process_format_string(char *out, const char *fmt, va_list args) {
	char *out_ptr = out;
	const char *fmt_ptr = fmt;

	while (*fmt_ptr != '\0') {
		if (*fmt_ptr == '%') {
			fmt_ptr++; // Move to the format specifier

			if (*fmt_ptr == 'd') {
				int i = va_arg(args, int);
				char num_str[20];
				int_to_str(i, num_str);	// Convert integer to string
				strcpy(out_ptr, num_str);	// Copy the number string to buffer
				out_ptr += strlen(num_str);
			}
			else if (*fmt_ptr == 's') {
				char *s = va_arg(args, char *);
				strcpy(out_ptr, s);
				out_ptr += strlen(s);
			}
		} else {
			*out_ptr++ = *fmt_ptr; // Copy other characters
		}
		fmt_ptr++;	// Move to the next character in format
	}
	*out_ptr = '\0';	// Null-terminate the buffer
	va_end(args);

	return out_ptr - out;	// Return the length of the string
}

int printf(const char *fmt, ...) {
	char buf[MAX_STRING_LEN];
	va_list args;
	va_start(args, fmt);

	int len = process_format_string(buf, fmt, args);

	for (int i = 0; i < len; i++) {
		putch(buf[i]);
	}
	
	return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	return process_format_string(out, fmt, args);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
