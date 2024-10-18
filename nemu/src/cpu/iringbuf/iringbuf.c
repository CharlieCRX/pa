#include "iringbuf.h"

void init_ringbuf(iringbuf *rb) {
	rb->head = NULL;
	rb->tail = NULL;
	rb->count = 0;
}

// 删除队列的首元素
void pop(iringbuf *rb) {
	if(rb->count == 0) return;

	Node *temp = rb->head;			// 保存当前头节点
	rb->head = rb->head->next;	// 更新头节点为下一个节点

	if (rb->head == NULL) {			// 仅有一个元素且删除后，尾节点也为空
		rb->tail = NULL;
	}

	rb->count--;
	free(temp);

}

// 在缓冲区末尾保存指令信息
void push(iringbuf *rb, const char *msg) {
	Node *new_node = (Node *) malloc(sizeof(Node));	// 分配新节点内存

	// 如果已经达到缓冲区最大容纳量
	if (rb->count == IRINGBUF_MAX_LEN) {
		pop(rb);		// 删除缓冲区中第一条指令信息
	}

	// 在末尾添加新的带有信息节点
	strncpy(new_node->message, msg, sizeof(new_node->message));	// 复制信息
	new_node->next = NULL;

	if (rb->count == 0) { // 如果缓冲区只有此一条新信息，则首尾皆更改为此信息
		rb->head = new_node;
		rb->tail = new_node;
	} else {
		rb->tail->next = new_node;	// 旧尾节点后添加此节点
		rb->tail = new_node;				// 更新尾节点指针
	}

	rb->count++;

}

void destroy_ringbuf(iringbuf *rb) {
	while( rb->count > 0 ) {
		pop(rb);
	}
}

void print_ringbuf(iringbuf *rb) {
	// 获取缓冲区的首元素地址
	Node *temp = rb->head;

	for (int i = 0; i < rb->count; i++) {
		// 最后一个元素的特殊输出
		if(i == rb->count - 1) {
			printf("  --> ");
		} else {
			printf("      ");
		}

		// 输出当前元素的信息
		printf("%s\n", temp->message);
		temp = temp->next;	// 更新临时节点为下一个节点
	}
}
