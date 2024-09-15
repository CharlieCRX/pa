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
	TK_HEX,
	TK_REG,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
	{"\\(", '('},
	{"\\)", ')'},
	{" +", TK_NOTYPE},    // spaces
	{"\\*", '*'},		  // multi
	{"\\/", '/'},		  // div
	{"\\+", '+'},         // plus
	{"\\-", '-'},		  // sub
	{"==", TK_EQ},        // equal
	{"(0x|0X)[0-9a-fA-F]+", TK_HEX}, // hexadecimal
	{"\\$[0-9a-zA-Z]+", TK_REG},     //	register
	{"[0-9]+", TK_NUM},   // decimal
};
#ifndef TEST
#define NR_REGEX ARRLEN(rules)
#else
#define NR_REGEX 11
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

//static Token tokens[32] __attribute__((used)) = {};
static Token tokens[65535] __attribute__((used)) = {};
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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
				#ifdef TEST
				printf("\n");
				#endif
        position += substr_len;

				switch (rules[i].token_type) {
					case TK_NOTYPE:
						break;
					case TK_REG:
						strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
						tokens[nr_token].type = rules[i].token_type;
						break;
					case TK_NUM:
					case TK_HEX:
						if(substr_len > 31) {
							substr_len = 31;
						}
						strncpy(tokens[nr_token].str, substr_start, substr_len);
          default:
						tokens[nr_token].type = rules[i].token_type;
#ifdef TEST
						//printf("tokens[%d].str = %s\n", nr_token, tokens[nr_token].str);
#endif
						nr_token += 1;// If it's a blank, nr_token not add 1
						break;
        }

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

		switch (tokens[i].type) {
			case TK_NUM:
			case TK_HEX:	
				printf("%s", tokens[i].str);
				break;
			default:
				printf("%c", tokens[i].type);
		}
	}
	printf("\n");
}
// ======================
// Function Prototypes
// ======================
#ifdef TEST
static uint32_t isa_reg_str2val(const char *s, bool *success){
	return 0;
}
#endif
static uint32_t convert_decimal(const char *str);
static uint32_t convert_hexadecimal(const char *str);
static uint32_t get_register_value(const char *str);

// Function pointer array for conversion functions
typedef uint32_t (*ConversionFunction)(const char *str);

static ConversionFunction conversion_function[] = {
	[TK_NUM] = convert_decimal,
	[TK_HEX] = convert_hexadecimal,
	[TK_REG] = get_register_value
	// Other types can be added here
};


// =========================
// Function Implementation
// =========================

/**
 * @brief Converts a decimal string to a 32-bit unsigned integer.
 * @param str The decimal string to convert.
 * @return The 32-bit unsigned integer representation of the string, or 0 on failure.
 */
static uint32_t convert_decimal(const char *str) {
	uint32_t result = 0;
	if(sscanf(str, "%u", &result) == 1) {
		return result;
	}
	assert(0);
}


/**
 * @brief Converts a hexadecimal string to a 32-bit unsigned integer.
 * @param str The hexadecimal string to convert.
 * @return The 32-bit unsigned integer representation of the string, or 0 on failure.
 */
static uint32_t convert_hexadecimal(const char *str) {
	uint32_t result = 0;
	if (sscanf(str, "%x", &result) == 1) {
		return result;
	}
	assert(0);
}

/**
 * @brief Gets the value(uint32_t) from a register named by the string.
 * @param str The name of the register.
 * @return The value of the register, or 0 on failure.
 */
static uint32_t get_register_value(const char *str) {
	bool success = true;
	uint32_t result = (uint32_t) isa_reg_str2val(str, &success);
	if (success) {
		return result;
	}
	assert(0);
}

/**
 * @brief Converts a token's value to a 32-bit unsigned integer based on its type.
 * @param token Pointer to the token whose value should be converted.
 * @return The 32-bit unsigned integer representation of the token's value, or 0 on failure.
 */
uint32_t convert_token_to_unsigned_num(const Token token){

	ConversionFunction convert_func = conversion_function[token.type];
	if (convert_func != NULL) {
		return convert_func(token.str);
	}
	assert(0);
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
	//printf("start check parenthess!**************\n");
	//print_tokens(p, q);
	//printf("token[%d].type = %c, tokenp[%d].type = %c\n", p, tokens[p].type, q, tokens[q].type);
	// 1. Check if the tokens at positions 'p' and 'q' are '(' and ')' respectively.
	if(tokens[p].type == '('&& tokens[q].type == ')') {
		p = p + 1;
		q = q - 1;
	} else {
	  //printf("this expr is not a (expr)\n");
		return false;
	}
	//print_tokens(p, q);
	// 2. For the inner expression (i.e., the tokens between 'p+1' and 'q-1'), 
	// verify that all parentheses are correctly matched.
	int left_brackets_num = 0;

	for (int i = p; i <= q && left_brackets_num >= 0; i++) {
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
	//printf("check paren now left brackets is %d\n", left_brackets_num);
	if (left_brackets_num != 0) {
		return false;
	}
	return true;
}

/**
 * @function find_corresponding_right_bracket_position()
 * @brief Finds the corresponding right bracket position in a tokens array.
 * 
 * This function takes the start position `p` in the tokens array, where `p` points to a left parenthesis `(`,
 * and the end position `q`. It then searches for the matching right parenthesis `)` within the range of the tokens array.
 * 
 * @param p The starting position in the tokens array, which points to a left parenthesis `(`.
 * @param q The ending position in the tokens array.
 * @return The position of the corresponding right parenthesis `)` if found.
 *         If no matching right parenthesis is found, the function returns -1.
 */
int find_corresponding_right_bracket_position(int p, int q) {
	//print_tokens(p, q);
	int position = -1;
	int left_brackets_num = 0;

	for (int i = p; i <= q; i++) {
		if(tokens[i].type == '(') {
			left_brackets_num += 1;
			//printf("%c index is  %d\n",tokens[i].type, i);
		} 
		else if (tokens[i].type == ')') {
			left_brackets_num -= 1;
			//printf("%c index is  %d\n",tokens[i].type, i);
		}

		if (left_brackets_num == 0) {
			position = i;
			break;
		}
	}
	return position;
}
/* 
* int locate_main_operator(int p, int q)
* Description: Retrieves the main operator location from an expression.
*	The main operator is typically the one 
*	with the lowest precedence that determines the structure of the 
*	expression.
*/
int locate_main_operator(int p, int q) {
	//printf("************START:locate_main_operator*********************\n");
	//print_tokens(p, q);
	int location = -1;
	for (int i = p; i <= q; i++) {
		// 1. Skip the operator if it is surrounded by parentheses `()`.
		if (tokens[i].type == '(') {
			// Find the corresponding ')'
			int corresponding_bracket_index = find_corresponding_right_bracket_position(i, q);
			if (corresponding_bracket_index == -1) assert(0);
			i = corresponding_bracket_index;
			//printf("corresponding right bracket index is %d\n", i);
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
	//printf("\ntokens[%d].type = %c\n",location, tokens[location].type);
	assert(location != -1);
	//printf("locate main operator is over now!\n\n");
	return location;
}

int32_t calc_apply(int op_type, int32_t val1, int32_t val2) {
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
	//printf("*********Start eval()!*********\n");
	//print_tokens(p, q);
	if(p > q) {
		// Negative number
		if (tokens[p].type == '-'){
			return 0;
		}
		// Bad expression
		assert(0);
	} 
	else if (p == q) {
		return convert_token_to_unsigned_num(tokens[p]);
	} 
	else if (check_parentheses(p, q) == true) {	
		return eval(p + 1, q -1);
	} 
	else {
		int operator_position = locate_main_operator(p, q);
		uint32_t val1 = eval(p , operator_position - 1);
		uint32_t val2 = eval(operator_position + 1, q);

		uint32_t result = calc_apply(tokens[operator_position].type, val1, val2);

		//printf("val1 = %d, val2 = %d, op = %c,result = %d\n", val1, val2, tokens[operator_position].type, result);
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
	char *str = "72 - (36)/((76))";
	make_token(str);
	int i = locate_main_operator(0, nr_token-1);
	assert(tokens[i].type == '-');
	printf("locate main operator is ok!\n");
}

void test_eval(){
	memset(tokens, 0, sizeof(tokens));
	nr_token = 0;
	//char *str = "((13-43+(49*(((2/49))-(19)+44)/97-15-6)/73-(((((60)))))-(((23)+((2)/55-(76))*50)+18/27*(((38)/64-40/45)))))/83";
	//char *str = "(49*(((2/49))-(19)+44)/97-15-6)/73";//0
	//char *str = "-(((23)+((2)/55-(76))*50)+18/27*(((38)/64-40/45)))";//3777
	//char *str = "(((((60)))))";//60
	//char *str = "(49*(((2/49))-(19)+44)/97-15-6)/73";	//0
	char *str = "(      -19) / 3 + 0X12";//-6 + 18 = 12
	make_token(str);
	//printf("nr_token = %d\n", nr_token);
	assert(eval(0, nr_token - 1) == 12);
	printf("eval test ok!\n");
}

void test_find_corresponding_right_bracket() {
	memset(tokens, 0, sizeof(tokens));
	nr_token = 0;
	char *str = "((76))";
	make_token(str);
	assert(find_corresponding_right_bracket_position(0, nr_token - 1) == 4);
	printf("corresponding is ok!\n");
}

void test_conversion_num() {
	char *str1 = "0x1234567812345678901234567890123";
	char *str2 = "4294967294"; 
	uint32_t result1 = convert_hexadecimal(str1);
	uint32_t result2 = convert_decimal(str2);
	printf("result1 = %u, 0x%x\n",result1, result1);
	printf("result2 = %u, 0x%x\n",result2, result2);
}

void test_convert_func() {
	char *str = "0x123456+4294967294+($r1)";
	make_token(str);
	print_tokens(0 ,nr_token - 1);

	for (int i = 0; i < nr_token; i++) {
		switch (tokens[i].type) {
			case TK_NUM:
			case TK_HEX:
			case TK_REG:
				uint32_t result = convert_token_to_unsigned_num(tokens[i]);
				printf("tokens[%d] = %u, 0x%x\n",i, result, result);
				break;
		}
	}
}

int main() {
	init_regex();
	//assert(make_token("0123+0x12345$reg12+3"));
	//print_tokens(0, nr_token - 1);
	//assert(make_token("(+-*///**++---)"));
	//test_check_parentheses();
	//test_locate_operator();
	//test_eval();
	//test_find_corresponding_right_bracket();
	//test_conversion_num();
	//test_convert_func();
	return 0;
}
#endif
