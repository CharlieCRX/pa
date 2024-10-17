#ifdef TEST
#include <assert.h>
#include "stdio.h"
#include "string.h"

void test_sprintf();

void test_sprintf() {
	char buf[100];

	// Test case 1: Formatting an integer
	int num = -32;
	sprintf(buf, "The number is %d.", num);
	assert(strcmp(buf, "The number is -32.") == 0);

	// Test case 2: Formatting a string
	const char *name = "Alice";
	sprintf(buf, "Hello, %s!", name);
	assert(strcmp(buf, "Hello, Alice!") == 0);

	// Test case 3: Mixing strings and numbers
	sprintf(buf, "%s is %d years old.", name, num);
	assert(strcmp(buf, "Alice is -32 years old.") == 0); 

}

int main() {
	test_sprintf();
	return 0;
}

#endif
