#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "iringbuf.h"

void test_init_ringbuf();
void test_push();
void test_pop();
void test_destory();

void test_init_ringbuf() {
	iringbuf *rb = (iringbuf *) malloc(sizeof(iringbuf));
	assert(rb != NULL);
	assert(rb->head == NULL);
	assert(rb->tail == NULL);
	assert(rb->count == 0);
	printf("Test init ringbuf is OK!\n");
	free(rb);
}

void test_push() {
	iringbuf *rb = (iringbuf *) malloc(sizeof(iringbuf));
	init_ringbuf(rb);
	// 1st push
	const char *msg1 = "this is the first message";
	push(rb, msg1);
	assert(rb->head != NULL);
	assert(rb->head == rb->tail);
	assert(strcmp(rb->head->message, msg1) == 0);
	assert(rb->count == 1);

	// 2rd push
	const char *msg2 = "this is the 2rd message";
	push(rb, msg2);
	assert(rb->head != rb->tail);
	assert(rb->count == 2);
	assert(strcmp(rb->head->message, msg1) == 0);
	assert(strcmp(rb->tail->message, msg2) == 0);

	// push until count == IRINGBUF_MAX_LEN
	push(rb, "3rd msg");
	push(rb, "4th msg");
	push(rb, "5th msg");
	assert(rb->count == IRINGBUF_MAX_LEN);
	assert(strcmp(rb->head->message, msg1) == 0);
	assert(strcmp(rb->tail->message, "5th msg") == 0);


	// push overflow IRINGBUF_MAX_LEN
	push(rb, "6 msg");
	push(rb, "7 msg");
	assert(rb->count == IRINGBUF_MAX_LEN);
	assert(strcmp(rb->head->message, "3rd msg") == 0);
	assert(strcmp(rb->tail->message, "7 msg") == 0);
	//print_ringbuf(rb);
	printf("Test push is OK!\n");
	destroy_ringbuf(rb);
}

void test_pop() {
	iringbuf *rb = (iringbuf *) malloc(sizeof(iringbuf));
	init_ringbuf(rb);
	push(rb, "msg1");
	push(rb, "msg2");
	push(rb, "msg3");
	assert(rb->count == 3);

	pop(rb);
	assert(rb->count == 2);
	assert(strcmp(rb->head->message, "msg2") == 0);
	assert(strcmp(rb->tail->message, "msg3") == 0);
	//print_ringbuf(rb);
	printf("Test pop is OK!\n");
}
int main() {
	test_init_ringbuf();
	test_push();
	test_pop();
	printf("All tests passed!\n");
	return 0;
}
#endif
