#ifndef TEST
#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#endif


#ifdef TEST
#include <stdio.h>
#endif
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t i = 0;
	size_t length = 0;
	while(s[i] != '\0') {
		length++;
		i++;
	}
	return length;
}

char *strcpy(char *dst, const char *src) {
	// Default: the length of dst is larger than src
	size_t i = 0;

	while(src[i] != '\0') {	// Overflow WARNNING
		dst[i] = src[i];
		i++;
	}

	return dst;	
}

char *strncpy(char *dst, const char *src, size_t n) {
	// Default: the n is smaller than length of dst 
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++) {
		dst[i] = src[i];
	}

	for( ; i < n; i++) {
		dst[i] = '\0';
	}

	return dst;
}

char *strcat(char *dst, const char *src) {
	size_t dest_len = strlen(dst);
	size_t i = 0;
	while(src[i] != '\0') {
		dst[dest_len + i] = src[i];
		i++;
	}
	dst[dest_len + i] = '\0';

	return dst;
}

int strcmp(const char *s1, const char *s2) {
	while ( *s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	size_t i;
	for ( i = 0; i < n ;i++ ) {
		if (s1[i] != s2[i]) {
			return (unsigned char) s1[i] - (unsigned char) s2[i];
		}

		if (s1[i] == '\0') {
			break;
		}
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	unsigned char *p = s;
	while(n--) {
		*p++ = (unsigned char) c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	unsigned char *d = dst;
	const unsigned char *s = src;

	if (d == s) {
		return dst;
	}

	// Copy from the beginning
	if (d < s) {

		while (n--) {
			*(d++) = *(s++);
		}
	}
	// If src is before dest, copy from the end to avoid overwriting
	else {
		d += n;
		s += n;

		while (n--) {
			*(--d) = *(--s);
		}
	}

	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	unsigned char *d = out;
	const unsigned char *s = in;

	while (n--) {
		*d++ = *s++;
	}

	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	const unsigned char *p1 = s1;
	const unsigned char *p2 = s2;

	size_t i;
	for (i = 0; i < n ; i++) {
		if (*p1 != *p2) 
			break;
	}
	return *p1 - *p2;
}

#endif

#ifdef TEST
char *s[] = {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	", World!\n",
	"Hello, World!\n",
	"#####"
};

char str1[] = "Hello";
char str[20];

int main() {
	strcat(strcpy(str, str1), s[3]);
	printf("str = %s\n", str);
	//if(memcmp(memset(str, '#', 5), s[5], 5) == 0) printf("TEST OK\n");
	printf("memset(str, '#', 5) = %s\n", (char *)memset(str, '#', 5));

	printf("memcmp(str, s[5], 5) = %d\n",memcmp(str, s[5], 5) );
	return 0;
}
#endif

