/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/
#ifndef TEST
#include "sdb.h"
#endif

#ifdef TEST
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#define word_t uint32_t
#endif
#define NR_WP 32
#define EXPR_LENGTH 32
typedef struct data {
	word_t address;
	char expr[EXPR_LENGTH];
	word_t old_value;
} DATA;

typedef struct watchpoint {
	int NO;
	DATA *data;
	struct watchpoint *next;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

// =====================
// Function Prototypes
// =====================
void init_wp_pool();
static void insert_tail(WP **head, WP *wp);
static void delete_head(WP **head);
static void delete_wp(WP **head, WP *key_wp);
static void print_list(WP **head);
WP* new_wp();
void free_wp(WP *wp);
void expr_watchpoint_create(char *e);
bool watchpoint_check_changes();
void print_wp();
// ============================
// Function Implementation
// ============================

static void print_list(WP **head) {
	if(*head == NULL) assert(0);
	printf("****************** print_list start! ************************\n");
	WP *temp = *head;
	while (temp != NULL) {
		printf("NO = %-2d,", temp->NO);
		printf("address = 0x%08x, expr = %s\n", temp->data->address, temp->data->expr);
		temp = temp->next;
	}
	printf("****************** print_list end! ************************\n");
}
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
		// Allocate memory for data
		wp_pool[i].data = (DATA *) malloc(sizeof(DATA));
		if (wp_pool[i].data == NULL) {
			printf("Memory allocation failed for wp_pool[%d].data\n", i);
			assert(0);
		}
		// Initialize data fields
		wp_pool[i].data->address = 0;
		strcpy(wp_pool[i].data->expr, "");
		wp_pool[i].data->old_value = 0;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
	#ifdef TEST
	printf("init success!\n");
	#endif
}

void clear_data(DATA *data) {
	if (data == NULL) assert(0);
	data->address = 0;
	data->old_value = 0;
	memset(data->expr, 0, sizeof(data->expr));
}
void clear_wp(WP *wp) {
	if (wp == NULL) assert(0);
	clear_data(wp->data);
	wp->next = NULL;
}


static void insert_tail(WP **head, WP *wp) {
	if (*head == NULL) {
		*head = wp;
		wp->next = NULL;
	}else {
		WP *temp = *head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = wp;
		wp->next = NULL;
	}
}

static void delete_wp(WP **head, WP *key_wp) {
	if (head == NULL || *head == NULL || key_wp == NULL) {
		assert(0);
	}

	if (*head == key_wp) {
		*head = (*head)->next;
		clear_wp(key_wp);
		return;
	}

	//Loop the list to ind the key wp
	WP *current = *head;
	while (current != NULL && current->next != key_wp) {
		current = current->next;
	}
	
	// If the node to be deleted is found
	if (current != NULL && current->next == key_wp) {
		current->next = key_wp->next;
		clear_wp(key_wp);
	} else {
		assert(0);
	}
}

static void delete_head(WP **head) {
	delete_wp(head, *head);
}

WP* new_wp() {
	if (free_ == NULL) {
		printf("There is no more free watchpoint! Please delete one!\n");
		return NULL;
	}
	WP *wp = free_;
	delete_head(&free_);
	insert_tail(&head, wp);
	return wp;
}
void free_wp(WP *wp) {
	WP *temp = wp;
	if (head == NULL) {
		printf("There is no more watchpoint! Please set a watchpoint first!\n");
		return ;
	}
	delete_wp(&head, wp);
	insert_tail(&free_, temp);
}

/*
 * @Function: expr_watchpoint_create
 * @Description: Inputs a string expression 'expr' and creates a new watchpoint, 
 *	if there is an available free node and the expression is valid
 * 
 * @Param: 
 *	- expr: A string representing the expression to be monitored by the watchpoint
 *
 */
void expr_watchpoint_create(char *e) {
	// Verify that the expression is legal
	bool success = false;
	word_t result = expr(e, &success);
	if (success) {
		// Allocate a new watchpoint structure 'wp' using `new_wp()`.
		WP *wp = new_wp();
		if (wp == NULL) return;
		// Store the input expression(Max lenth is EXPR_LENGTH) in the newly allocated watchpoint.
		strncpy(wp->data->expr, e, EXPR_LENGTH - 1);
		
		// Assign the result to `old_value` in the watchpoint
		wp->data->old_value = result;
	} else {
		printf("Your expression is invalid!\n");
	}
}

void print_wp(WP *wp) {
	printf("watchpoint %d: %s\n", wp->NO, wp->data->expr);
}

void print_comparision_in_wp(WP *wp, word_t result) {
	print_wp(wp);
	printf("\nOld value = " FMT_WORD "\n", wp->data->old_value);
	printf("New value = " FMT_WORD "\n", result);
}
/*
 * @Function: watchpoint_check_changes
 * @Description: 
 *		Retrieves all watchpoints whose evaluated expression values have changed.
 * @Returns:
 *	- `true`  if any watchpoint's expression value has changed.
 *  - `false` if no changes were detected.
*/
bool watchpoint_check_changes() {
	bool has_changed = false;
	WP *current = head;
	// Traverse through all existing watchpoints.
	while (current != NULL) {
		// For each watchpoint, retrieve its associated expression.
		bool success = true;
		word_t result = expr(current->data->expr, &success);

		// Compare the result with the previous value (`old_value`). 
		if (result != current->data->old_value) {
			// Output the comparison between the old value and result
			print_comparision_in_wp(current, result);
			// Replace the old value with the result
			current->data->old_value = result;
			has_changed = true;
		}
		current = current->next;
	}
	return has_changed;
}

void print_watchpoints() {
	print_list(&head);
}
// =====================
// Test Function
// =====================

void init_data() {
	init_wp_pool();
	WP *wp = &wp_pool[0];
	wp->data->address = 0x12345678;
	strcpy(wp->data->expr, "1234+2345");
}

void test_clear_data() {
	init_data();

	WP *wp = &wp_pool[0];
	clear_data(wp->data);
	wp->next = &wp_pool[1];
	printf("clear data!*****************\n");
	print_list(&free_);
}

void test_wp() {
	init_data();
	WP *wp = new_wp();

	wp->data->address = 0x345678;
	strcpy(wp->data->expr, "test_wp1() + 1234 = 123");

	wp = new_wp();
	wp->data->address = 0x12344321;
	strcpy(wp->data->expr, "test_wp2() + 1234 = 123");
	print_list(&head);

	printf("\n\nnow free_ is:");
	print_list(&free_);

	free_wp(&wp_pool[0]);
	printf("free_wp now: head is \n");
	print_list(&head);

	for (int i = 0; i < 33; i++) {
		new_wp();
	}
	free_wp(&wp_pool[11]);
	printf("\n\nnow free is:\n");
	print_list(&free_);
}
#ifdef TEST
int main() {
	//test_clear_data();
	test_wp();
	return 0;
}
#endif
