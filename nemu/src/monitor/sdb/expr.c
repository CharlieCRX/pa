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
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#else
#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#endif
enum {
  TK_NOTYPE = 256, 
	TK_EQ,
	TK_LEFT_BRACKET,
	TK_RIGHT_BRACKET,
	TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
	{"\\(", TK_LEFT_BRACKET},
	{"\\)", TK_RIGHT_BRACKET},
  {" +", TK_NOTYPE},    // spaces
	{"\\*", '*'},					// multi
	{"\\/", '/'},					// div
  {"\\+", '+'},         // plus
	{"\\-", '-'},					// sub
  {"==", TK_EQ},        // equal
	{"[0-9]+", TK_NUM}, // integer
};
#ifndef TEST
#define NR_REGEX ARRLEN(rules)
#else
#define NR_REGEX 9
#define Log printf
#define panic printf
#endif
static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
	#ifdef TEST
	printf("rules[8].regex = %s\n",rules[8].regex);
	#endif
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
				#ifdef TEST
				printf("\n");
				#endif
        position += substr_len;

				switch (rules[i].token_type) {
					case TK_NOTYPE:
						break;
					case TK_NUM:
						if(substr_len > 31) {
							substr_start = "-1";
							substr_len = 2;
						}
						strncpy(tokens[nr_token].str, substr_start, substr_len);

          default:
						tokens[nr_token].type = rules[i].token_type;
#ifdef TEST
						printf("tokens[%d].str = %s\n", nr_token, tokens[nr_token].str);
#endif
						break;
        }
				nr_token += 1;

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

#ifndef TEST
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
#endif

#ifdef TEST
int main() {
	init_regex();
	assert(make_token("(123+789-323)"));
	//assert(make_token("(+-*///**++---)"));
	return 0;
}
#endif
