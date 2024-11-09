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

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
	printf("\nstart - isa_raise_intr: no = %d, epc = 0x%08x\n", NO, epc);
  /* Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
	// 保存程序状态
	cpu.csrs.mepc = epc;
	cpu.csrs.mcause = NO;

	// 跳转异常处理入口地址
	printf("end - isa_raise_intr: mcause = %d, mepc = 0x%08x\n", cpu.csrs.mcause, cpu.csrs.mepc);
	return cpu.csrs.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
