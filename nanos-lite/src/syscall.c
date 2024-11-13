#include <common.h>
#include "syscall.h"

void strace(Context *c) {
  Log("strace: Syscall num: %d, param(a0, a1, a2) = (%d, %d, %d), ret  = %d\n", 
  c->GPR1, c->GPR2, c->GPR3, c->GPR4, c->GPRx);
}
void sys_yield(Context *c) {
  yield();
  c->GPRx = 1;
}
void sys_exit() {
  halt(SYS_exit);
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit : sys_exit(c);  strace(c); break;
    case SYS_yield: sys_yield(c); strace(c);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
}
