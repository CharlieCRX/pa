#ifndef RISCV_H__
#define RISCV_H__

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

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
static inline uint32_t inl(uintptr_t addr) { return *(volatile uint32_t *)addr; }

static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outl(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }

#define PTE_V 0x01
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40
#define PTE_D 0x80

enum { MODE_U, MODE_S, MODE_M = 3 };
#define MSTATUS_MXR  (1 << 19)
#define MSTATUS_SUM  (1 << 18)

#if __riscv_xlen == 64
#define MSTATUS_SXL  (2ull << 34)
#define MSTATUS_UXL  (2ull << 32)
#else
#define MSTATUS_SXL  0
#define MSTATUS_UXL  0
#endif

#endif
