#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include <stdint.h>
extern char test_address[10];
#endif

#ifndef TEST
#include "watchpoint.h"
#include "sdb.h"
#endif

typedef enum {
	CALL,
	RET
} function_state;

/*
	格式为：
	0x8000000c: call      [_trm_init@0x80000260]
	^           ^					^	         ^
	|						|					|					 |
	pc					state	    name       addr
*/
typedef struct Node{
	uint32_t pc;
	function_state state;
	char name[20];
	uint32_t func_addr;
	struct Node *next;
} Node;

typedef struct {
	Node *head;
	Node *tail;
	int count;
} ftrace;

void init_ftrace(ftrace *ft);
void push(Node *node, ftrace *ft);
void pop(ftrace *ft);
void assemble_node_info(Decode *s, Node *node);
void trace_function_call_and_ret(Decode *s, Node *new_node, ftrace *ft);
void destroy_ftrace(ftrace *ft);
void print_ftrace(ftrace *ft);
