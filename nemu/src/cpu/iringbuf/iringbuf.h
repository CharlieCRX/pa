#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifndef TEST
#include "watchpoint.h"
#include "sdb.h"
#endif
#define IRINGBUF_MAX_LEN 5 

typedef struct Node {
	char message[128];
	struct Node *next;
} Node;

typedef struct {
	Node *head;
	Node *tail;
	int count; // 链表元素的数量
} iringbuf;


void init_ringbuf(iringbuf *rb);
void pop(iringbuf *rb);
void push(iringbuf *rb, const char *msg);
void destroy_ringbuf(iringbuf *rb);
void print_ringbuf(iringbuf *rb);
