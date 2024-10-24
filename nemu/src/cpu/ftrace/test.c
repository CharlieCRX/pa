#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ftrace.h"

void init_decode(Decode *s, uint32_t pc, uint32_t dnpc, int val) {
	s->pc = pc;
	s->dnpc = dnpc;
	s->isa.inst.val = val;
}


void test_init() {
	ftrace *ft = (ftrace *) malloc (sizeof(ftrace));
	init_ftrace(ft);
	assert(ft->head == NULL);
	assert(ft->tail == NULL);
	assert(ft->count == 0);
	free(ft);
	printf("test init ok!\n");
}


void test_push() {
	ftrace *ft = (ftrace *) malloc (sizeof (ftrace));
	init_ftrace(ft);

	// 1st
	Decode *s1 = (Decode *) malloc (sizeof (Decode));
	Node *node1 = (Node *) malloc(sizeof (Node));
	init_decode(s1, 0x80000000, 0x80000034, 0x6f701000);
	assemble_node_info(s1, node1);
	assert(node1->pc == 0x80000000);
	assert(node1->state == CALL);
	assert(strcmp(node1->name, test_address) == 0);
	assert(node1->func_addr == 0x80000034);

	push(node1, ft);
	assert(ft->head != NULL);
	assert(ft->head->pc == 0x80000000);
	assert(ft->head->state == CALL);
	assert(strcmp(ft->head->name, test_address) == 0);
	assert(ft->head->func_addr == 0x80000034);
	assert(ft->head == ft->tail);
	assert(ft->count == 1);

	// 2rd

	Decode *s2 = (Decode *) malloc (sizeof (Decode));
	Node *node2 = (Node *) malloc(sizeof (Node));
	init_decode(s2, 0x90000000, 0x90000034, 0x9f701000);
	assemble_node_info(s2, node2);
	assert(node2->pc == 0x90000000);
	assert(node2->state == CALL);
	assert(strcmp(node2->name, test_address) == 0);
	assert(node2->func_addr == 0x90000034);

	push(node2, ft);
	assert(ft->head != NULL);
	assert(ft->tail->pc == 0x90000000);
	assert(ft->tail->state == CALL);
	assert(strcmp(ft->tail->name, test_address) == 0);
	assert(ft->tail->func_addr == 0x90000034);
	assert(ft->head != ft->tail);
	assert(ft->count == 2);

	// pop
	pop(ft);

	assert(ft->head != NULL);
	assert(ft->tail->pc == 0x90000000);
	assert(ft->tail->state == CALL);
	assert(strcmp(ft->tail->name, test_address) == 0);
	assert(ft->tail->func_addr == 0x90000034);
	assert(ft->head == ft->tail);
	assert(ft->count == 1);


	destroy_ftrace(ft);
	printf("test push ok!\n");
}

void test_trace_function() {
	ftrace *ft = (ftrace *) malloc (sizeof (ftrace));
	Decode *s1 = (Decode *) malloc (sizeof (Decode));
	Node *node = (Node *)   malloc (sizeof (Node));
	init_decode(s1, 0x80000000, 0x80000034, 0x6f701000);
	trace_function_call_and_ret(s1, node, ft);

	assert(ft->head != NULL);
	assert(ft->head->pc == 0x80000000);
	assert(ft->head->state == CALL);
	assert(strcmp(ft->head->name, test_address) == 0);
	assert(ft->head->func_addr == 0x80000034);
	assert(ft->head == ft->tail);
	assert(ft->count == 1);
	print_ftrace(ft);
	destroy_ftrace(ft);

	assert(ft->head == ft->tail);
	assert(ft->head == NULL);
	assert(ft->count == 0);

	printf("trace is ok!\n");	
}

int main() {
	test_init();
	test_push();
	test_trace_function();
	return 0;
}
#endif
