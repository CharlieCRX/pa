/*
#include "ftrace.h"
#include <assert.h>
#ifdef TEST
char test_address[10] = "test_func1"; 
char *get_function_name_by_address(uint32_t addr) {
	return test_address;
}
int is_call_instruction(uint32_t value) {
	return 1;
}

int is_ret_instruction(uint32_t value) {
	return 0;
}
#endif
void init_ftrace(ftrace *ft) {
	ft->head = NULL;
	ft->tail = NULL;
	ft->count = 0;
}

// 在ftrace末尾记录函数调用信息
void push(Node *node, ftrace *ft) {
	if (node != NULL) {
		if (ft->count == 0) {
			ft->head = node;
			ft->tail = node;
		} else {
			ft->tail->next = node;
			ft->tail = node;
		}

		ft->count++;
	}
}

// 删除首元素
void pop(ftrace *ft) {
	if (ft->count == 0) return;

	Node *temp = ft->head;
	ft->head = ft->head->next;

	ft->count--;
	if(ft->count == 0) {
		ft->tail = NULL;
	}
	free(temp);
}

void assemble_node_info(Decode *s, Node *node) {

	// 记录当前执行指令的地址
	node->pc = s->pc;

	// 判断是调用还是返回操作
	if (is_call_instruction(s->isa.inst.val)) {

		node->state = CALL;

	} else if (is_ret_instruction(s->isa.inst.val)) {

		node->state = RET;

	} else {
		assert(0);
	}

	// 获取函数名称
	char *func_name = get_function_name_by_address(s->dnpc); //TODO
	strncpy(node->name, func_name, sizeof(node->name));

	// 获取函数所在地址
	node->func_addr = s->dnpc;

	// Next
	node->next = NULL;

}

// 记录指令的调用或者返回情况
void trace_function_call_and_ret(Decode *s, Node *new_node, ftrace *ft) {
	assemble_node_info(s, new_node);
	push(new_node, ft);
}


void destroy_ftrace(ftrace *ft) {
	while (ft->count > 0) {
		pop(ft);
	}
}

void print_ftrace(ftrace *ft) {
	Node *temp = ft->head;
	char operation[5];
	for (int i = 0; i < ft->count; i++) {
		if (temp->state == CALL) {
			strcpy(operation, "call");
		} else {
			strcpy(operation, "ret ");
		}

		printf("0x%08x: %s[%s@0x%08x]\n", temp->pc, operation, temp->name, temp->func_addr);
		temp = temp->next;
	}
}
*/
