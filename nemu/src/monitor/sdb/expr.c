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
#include <stdint.h>
#endif
enum {
  TK_NOTYPE = 256, 
	TK_EQ,
	TK_NUM,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
	{"\\(", '('},
	{"\\)", ')'},
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
	memset(tokens, 0, sizeof(tokens));

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);
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
						//printf("tokens[%d].str = %s\n", nr_token, tokens[nr_token].str);
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

/*
* Function: print_tokens
* Description: Outputs the elements of the 'tokens' array within the specified range.
* Parameters:
* - p: The starting index (inclusive) for output. can start from 0.
* - q: The ending index (exclusive) for output. 
*			q should be less than or equal to the length of the 'tokens' array.
*/
void print_tokens(int p, int q) {
	for(int i = p; i <= q; i++) {
		if(tokens[i].type == TK_NUM) {
			printf("%s", tokens[i].str);
		} else {
			printf("%c", tokens[i].type);
		}
	}
	printf("\n");
}
	


/*
 * check_parentheses - Check if the token array is a parenthesized expression.
 * @param p: The left boundary index of the token array.
 * @param q: The right boundary index of the token array.
 
 * This function checks if the token array from index 'p' to 'q' (inclusive)
 * is enclosed by a pair of matching parentheses. If the tokens form a valid
 * parenthesized expression, the function returns true. Otherwise, it returns false.
*/
bool check_parentheses(int p,int q) {
	// 1. Check if the tokens at positions 'p' and 'q' are '(' and ')' respectively.
	if(tokens[p].type == '('&& tokens[q].type == ')') {
		p = p + 1;
		q = q - 1;
	} else {
		return false;
	}
	//print_tokens(p, q);
	// 2. For the inner expression (i.e., the tokens between 'p+1' and 'q-1'), 
	// verify that all parentheses are correctly matched.
	int left_brackets_num = 0;

	for (int i = p; i < q && left_brackets_num >= 0; i++) {
		if(tokens[i].type == '(') {
			left_brackets_num += 1;
		}

		if(tokens[i].type == ')') {
			left_brackets_num -= 1;
		}
	}
	/*
	 * If left_brackets_num is greater than 0, it indicates there are extra left parentheses '('
	 * that cannot be matched with corresponding right parentheses.
	 * If left_brackets_num is less than 0, it indicates there are extra right parentheses ')'.
	 */
	if (left_brackets_num != 0) {
		return false;
	}
	return true;
}
/* 
* int locate_main_operator(int p, int q)
* Description: Retrieves the main operator location from an expression.
*	The main operator is typically the one 
*	with the lowest precedence that determines the structure of the 
*	expression.
*/
int locate_main_operator(int p, int q) {
	int location = -1;
	for (int i = p; i <= q; i++) {
		// 1. Skip the operator if it is surrounded by parentheses `()`.
		if (tokens[i].type == '(') {
			while(tokens[i].type != ')') {
					i++;
					// no matches right brancket
					if (i > q) assert(0);
			}
		}
		// 2. Replace the recorded operator if the next operator is `+` or `-`.
		else if (tokens[i].type == '+' || tokens[i].type == '-') {
			// "--1" and "1-1-1"
			// Two consecutive negative signs,choose the first one as the main operator
			if(location != -1 && (location == i - 1) && tokens[location].type == '-')  {
				continue;
			}
			location = i;
		}
		// 3. If both the recorded and next operators are `*` or `/`, update the recorded operator. 
		// If the location is uninitialized, update the record operator, too.
		else if (tokens[i].type == '*' || tokens[i].type == '/') {
			// Uninitialized location
			if (location == -1) {
				location = i;
			}
			// Also replaces if the previous operator has the same precedence
			else if (tokens[location].type == '*' || tokens[location].type == '/') {
				location = i;
			}
		}
	}
	//printf("tokens[%d].type = %c\n",location, tokens[location].type);
	assert(location != -1);
	return location;
}

//str to num, if not num, return 0
static uint32_t string_to_num(char *str) {
	int i = 0;
	uint32_t result = 0;

	while(str[i] != '\0') {
		if(str[i] >= '0' && str[i] <= '9') {
			result = 10 * result + (str[i] - '0');
		}
		else{
			return 0;
		}
		i++;
	}
	return result;
}


uint32_t calc_apply(int op_type, uint32_t val1, uint32_t val2) {
	switch (op_type) {
		case '+': return val1 + val2;
		case '-': return val1 - val2;
		case '*': return val1 * val2;
		case '/':
			if(val2 != 0) {
				return val1 / val2;
			}
		default: assert(0);
	}
	return 0;
}

/*
*	Function: uint32_t eval(int p,int q)
* Description:Evaluates the value of an expression represented by tokens.
*
* Parameters:
*   p - the starting index of the expression in the tokens array.
*   q - the ending index of the expression in the tokens array.
*
* Returns:
*   The computed value of the expression as a 32-bit unsigned integer (uint32_t).
*/
uint32_t eval(int p, int q) {
	if(p > q) {
		// Negative number
		if (tokens[p].type == '-'){
			return 0;
		}
		// Bad expression
		assert(0);
	} 
	else if (p == q) {
		return string_to_num(tokens[p].str);
	} 
	else if (check_parentheses(p, q) == true) {	
		return eval(p + 1, q -1);
	} 
	else {
		int operator_position = locate_main_operator(p, q);
		uint32_t val1 = eval(p , operator_position - 1);
		uint32_t val2 = eval(operator_position + 1, q);

		uint32_t result = calc_apply(tokens[operator_position].type, val1, val2);

		return result;

	}
}




#ifndef TEST
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  return eval(0, nr_token - 1);
}
#endif

#ifdef TEST
void test_check_parentheses() {
	memset(tokens, 0, sizeof(tokens));
	nr_token = 0;
	//char *str = "(123 + 456 +12)";
	//char *str = "()";
	char *str = "(1239991()-)";
	make_token(str);
	assert(check_parentheses(0, nr_token -1));
	printf("check ok!\n");
}

void test_locate_operator(){
	memset(tokens, 0, sizeof(tokens));
	nr_token = 0;
	char *str = "(1+(3-5)/3*2";
	make_token(str);
	int i = locate_main_operator(0, nr_token-1);
	assert(tokens[i].type == '*');
	printf("locate main operator is ok!\n");
}

void test_eval(){
	memset(tokens, 0, sizeof(tokens));
	nr_token = 0;
	char *str = "1--4";
	make_token(str);
	assert(eval(0, nr_token - 1) == 5);
	printf("eval test ok!\n");
}
int main() {
	init_regex();
	//assert(make_token("(123+789-323)"));
	//assert(make_token("(+-*///**++---)"));
	//test_check_parentheses();
	//test_locate_operator();
	test_eval();
	return 0;
}
#endif
