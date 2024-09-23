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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "watchpoint.h"
#include "memory/vaddr.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

/*
	Input: string
	output: if str is not a num string, return 0
					else return the real num value of the string
*/
static int string_to_num(char *str) {
	int i = 0;
	int result = 0;
	/* Loop through the string*/
	while(str[i] != '\0') {
		// Check if the character is a digit (0-9)
		if (str[i] >= '0' && str[i] <= '9') {
			result = result * 10 + (str[i] - '0');
		} else {
			printf("Warning: %s is not a num!\n", str);
			return 0;
		}
		
		i++;
	}
	return result;
}

vaddr_t hex_string_to_vaddr(const char* hex_str) {
	vaddr_t result = 0;
	sscanf(hex_str, "%x", &result);
	return result;
}


static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


uint64_t get_steps_from_args() {
	char *num = strtok(NULL, " ");
	
  /* no argument given */
	if (num == NULL) {
		return 1;
	} 

	uint64_t result = 0;
	int i = 0;

	/* Loop through the string*/
	while(num[i] != '\0') {
		// Check if the character is a digit (0-9)
		if (num[i] >= '0' && num[i] <= '9') {
			result = result * 10 + (num[i] - '0');
		} else {
			//Return 1 if a non-digit character is found
			return 1;
		}
		
		i++;
	}

	return result;
}

			
static int cmd_step(char *args) {
	/* get the num of steps from args*/
	uint64_t n = get_steps_from_args();
	cpu_exec(n);
	return 0;
}



static int cmd_q(char *args) {
	nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_info(char *args);

static int cmd_scan_memory(char *args);

static int cmd_eval(char *args);

static int cmd_watchpoint_create(char *args);

static int cmd_watchpoint_delete(char *args); 
static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{ "si", "Executes n steps in the program", cmd_step},
	{ "info", "Prints the current infomation of registers or watchpoints", cmd_info},
	{ "x", "Scans memory starting from a given address and outputs N consecutive 4-byte values.", cmd_scan_memory},
	{ "p", "Evaluate the expression EXPR to obtain its value, including printing register values and dereferencing pointers.", cmd_eval},
	{ "w", "Set a watchpoint with an valid expression.", cmd_watchpoint_create},
	{ "d", "Delete a watchpoint with a specific number.", cmd_watchpoint_delete},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}


static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");

	if (arg == NULL || strcmp(arg, "r") == 0) {
		printf("print register!\n\n\n\n");
		isa_reg_display();
	}

	if (strcmp(arg, "w") == 0) {
		print_watchpoints();
	}
	return 0;
}

static int cmd_scan_memory(char *args) {
	char *lines_str = strtok(NULL, " ");
	if (lines_str == NULL) {
		printf("Please input N!\n");
		return 0;
	}

	char *addr_str = strtok(NULL, " ");
	if (addr_str == NULL) {
		printf("Please input address!\n");
		return 0;
	}

	int lines = string_to_num(lines_str); //1,2,3...N
	vaddr_t addr_hex = hex_string_to_vaddr(addr_str);

	if (addr_hex < CONFIG_MBASE) {
		printf("Invalid address!\n");
		return 0;
	}

	for (int i = 0; i < lines; i++) {
		printf(FMT_WORD ":", addr_hex + i*4);
		for (int j = 0; j < 4; j++) {
			uint8_t data = vaddr_read(addr_hex + 4*i + j, 1);
			//4 bytes per line
			//uint8_t data = *addr_hex;
			printf(" %02x", data);
		}
		printf("\n");
	}
	return 0;
}

static int cmd_eval(char *args) {
	bool success = true;
	word_t result = expr(args, &success);
	if (success) {
		printf("result = %u(0x%08x)\n", result, result);
		return 0;
	}
	assert(0);
	return 0;
}

static int cmd_watchpoint_create(char *args) {
	expr_watchpoint_create(args);
	return 0;
}

static int cmd_watchpoint_delete(char *args) {
	int no = string_to_num(args);
	watchpoint_remove_by_id(no);
	return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
