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

#include <common.h>
#include "/home/crx/study/ics2023/nemu/src/monitor/sdb/sdb.h"
void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();
/*
 * Func:test_gen_expr 
 * @desciption: Retrieve and validate arithmetic expressions from the test file.
 * This function reads the test file located at `nemu/tools/gen-expr/input`,
 * where each line is formatted as `result expr`. It evaluates each arithmetic
 * expression (expr) and compares the evaluated result with the expected value (result).
 */
void test_gen_expr() {
	// 1. Read the test file into the buffer `buf`
	FILE *fp = fopen("/home/crx/study/ics2023/nemu/tools/gen-expr/input", "r");
	if (fp == NULL) {
		printf("ERROR:Can not open the test file!\n");
		return;
	}

	char buf[65536]; // Buffer to store each line of the file
	word_t result;
	char expr_str[65535]; // Stores the string of the expression 

	// 2.Loop through each line to read the result and expression
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		printf("this line:%s\n", buf);
		// Extract `result` and `expr_str` from the buffer
		sscanf(buf, "%u %s", &result, expr_str);

		// 2.1 Call the `expr()` function to evaluate the result of the expression
		bool flag = true;
		word_t calc_value = expr(expr_str, &flag);

		// 2.2 Compare the calculated result with the expected result
		assert(result == calc_value);
	}
}

int main(int argc, char *argv[]) {
#ifdef TEST 
	test_gen_expr();
#endif
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
