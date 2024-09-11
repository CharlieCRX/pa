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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static char temp[100];
static void gen_rand_expr();
uint32_t choose(uint32_t n) {
	uint32_t num = (uint32_t) rand() % n;
	return num;
}

void write_to_buf(char *source) {
	if (strlen(buf) + strlen(source) < sizeof(buf) - 100){
		strcat(buf, source);
	}
}
		
void gen_num() {
	int rand_num = rand() % 100;
	sprintf(temp, "%d", rand_num);
	strcat(buf, temp);
}

void gen(const char ch) {
	sprintf(temp, "%c", ch);
	write_to_buf(temp);
}

char gen_rand_op() {
	char op[4] = {'+', '-', '*', '/'}; 
	int valid_rand_num = rand() % 4;
	gen(op[valid_rand_num]);
	return op[valid_rand_num];
}
int evaluate_last_generated_expr() {
	int last_num = rand() % 99 + 1;
	return last_num;
}

static void gen_rand_expr() {
	switch(choose(3)) {
		case 0: gen_num(); break;
		case 1: gen('('); gen_rand_expr(); gen(')'); break;
		case 2: 
			gen_rand_expr(); 
			char op = gen_rand_op(); 
			if (op == '/') {
				int divisor = evaluate_last_generated_expr();
				sprintf(temp, "%d", divisor);
				write_to_buf(temp);
			} 
			else {
				gen_rand_expr(); 
			}
			break;
	}
}
#ifndef TEST
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		buf[0] = '\0';
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
#endif

#ifdef TEST
void test_get_opt() {
	for(int i = 0; i < 100; i++) {
		gen_rand_op();
		printf("buf[%d] = %c\n", i, buf[i]);
	}
	printf("\n");
}

int main() {
	test_get_opt();
	return 0;
}
#endif
