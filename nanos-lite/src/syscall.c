#include <common.h>
#include "syscall.h"

void strace(Context *c) {
  Log("strace: Syscall num: %d, param(a0, a1, a2) = (%d, %d, %d), ret  = %d\n", 
  c->GPR1, c->GPR2, c->GPR3, c->GPR4, c->GPRx);
}
void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}
void sys_exit(int status) {
  halt(status);
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit : strace(c); sys_exit(SYS_exit);   break;
    case SYS_yield: strace(c); sys_yield(c);  break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
}
