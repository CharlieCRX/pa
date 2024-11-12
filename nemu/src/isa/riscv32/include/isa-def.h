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

#ifndef __ISA_RISCV_H__
#define __ISA_RISCV_H__

#include <common.h>

// 用于控制和监控 CPU 状态的特殊寄存器
typedef struct control_and_status_registers {
	word_t mtvec;  // 异常入口地址
	word_t mepc;   // 触发异常的PC
	word_t mstatus;// 处理器的状态
	word_t mcause; // 触发异常的原因
}CSRs;

// 常用 CSR 地址
typedef enum {
  // Machine Trap Setup
	CSR_MSTATUS = 0x300,  // mstatus
	CSR_MTVEC   = 0x305,  // mtvec

  // Machine Trap Handling
	CSR_MEPC    = 0x341,	// mepc
	CSR_MCAUSE  = 0x342,  // mcause
}csr_id;

typedef struct {
  word_t gpr[MUXDEF(CONFIG_RVE, 16, 32)];
  vaddr_t pc;
	CSRs csrs;
} MUXDEF(CONFIG_RV64, riscv64_CPU_state, riscv32_CPU_state);

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} MUXDEF(CONFIG_RV64, riscv64_ISADecodeInfo, riscv32_ISADecodeInfo);


#include <stdint.h>

typedef enum {
  //mcause 的最高位在发生中断时置 1,发生同步异常时置 0
    INSTRUCTION_ADDRESS_MISALIGNED = 0,  // 指令地址未对齐
    INSTRUCTION_ACCESS_FAULT       = 1,  // 指令访问故障
    ILLEGAL_INSTRUCTION            = 2,  // 非法指令
    BREAKPOINT                     = 3,  // 断点
    LOAD_ADDRESS_MISALIGNED        = 4,  // 加载地址未对齐
    LOAD_ACCESS_FAULT              = 5,  // 加载访问故障
    STORE_ADDRESS_MISALIGNED       = 6,  // 存储地址未对齐
    STORE_ACCESS_FAULT             = 7,  // 存储访问故障
    ENVIRONMENT_CALL_FROM_U_MODE   = 8,  // 用户模式的环境调用
    ENVIRONMENT_CALL_FROM_S_MODE   = 9,  // 管理模式的环境调用（若存在）
    ENVIRONMENT_CALL_FROM_M_MODE   = 11, // 机器模式的环境调用
    INSTRUCTION_PAGE_FAULT         = 12, // 指令页面故障
    LOAD_PAGE_FAULT                = 13, // 加载页面故障
    STORE_PAGE_FAULT               = 15, // 存储页面故障
    // 中断的最高位为1，这里使用更大的数值表示
    INTERRUPT_MACHINE_TIMER        = 0x80000007, // 机器定时器中断
    INTERRUPT_MACHINE_EXTERNAL     = 0x8000000b  // 机器外部中断
} RiscV_ExceptionCode;  


#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
