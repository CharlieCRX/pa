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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
#define gpr(idx) (cpu.gpr[check_reg_idx(idx)])
void isa_reg_display() {
	printf("pc = 0x%08x\n", nemu_state.halt_pc);
	for(int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++) {
		if (gpr(i) == 0) {
			printf("%-3s = %d\n", regs[i], gpr(i));
		}
		else {
			printf("%-3s = 0x%08x\n", regs[i], gpr(i));
		}
	}
}

/**
 * @brief Finds the index of a given register name in the predefined array of register names. 
 * @param name The register name to search for in the array.
 * @return The index of the register name if found, otherwise -1.
 */
static int reg_idx_by_name(const char *name) {
	int num_regs = MUXDEF(CONFIG_RVE, 16, 32);

	// Iterate over the register names array to find a match
	for (int i = 0; i < num_regs; i++) {
		if (strcmp(name, regs[i]) == 0) {
			return i;
		}
	}
	return -1;
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int idx = reg_idx_by_name(s);
	if(idx != -1) {
		*success = true;
		return (word_t)gpr(idx);
	}
	*success = false;
	return -1;
}
