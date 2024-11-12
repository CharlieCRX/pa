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

/** 模拟硬件异常响应机制*/
word_t isa_raise_intr(word_t NO, vaddr_t epc) {

	// 将当前PC值保存到mepc寄存器
	cpu.csrs.mepc = epc;

  // 在mcause寄存器中设置异常号
	cpu.csrs.mcause = NO;

	// 从mtvec寄存器中取出异常入口地址
	return cpu.csrs.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
