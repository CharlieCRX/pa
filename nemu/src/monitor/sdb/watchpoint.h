#ifndef WP_H
#define WP_H
#include <stdbool.h>

void expr_watchpoint_create(char *expr);
bool watchpoint_check_changes();
void print_watchpoints();

#endif