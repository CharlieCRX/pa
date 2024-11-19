#include <common.h>
#include "syscall.h"
#include "fs.h"

void strace(Context *c) {
  Log("strace: Syscall num: %d, param(a0, a1, a2) = (0x%x, 0x%x, 0x%x)", 
  c->GPR1, c->GPR2, c->GPR3, c->GPR4);
}
void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}
void sys_exit(Context *c) {
  halt(c->GPR2);
}

void sys_write(Context *c) {
  int fd       = (int)c->GPR2;
  char *buf    = (char *)c->GPR3;
  size_t count = (size_t)c->GPR4;

  if (fd == 1 || fd == 2) {
	  for (int i = 0; i < count; i++) {
	  	putch(buf[i]);
	  }
  }

  c->GPRx = count;
}

void sys_sbrk(Context *c) {
  c->GPRx = 0;
  strace(c);
}

void sys_open(Context *c) {
  const char *pathname = (char *)c->GPR2;
  int flags = (int) c->GPR3;
  int mode = (int) c->GPR4;

  c->GPRx = fs_open(pathname, flags, mode);
}

void sys_read(Context *c) {
  int fd = (int) c->GPR2;
  void *buf = (void *)c->GPR3;
  size_t len = (size_t) c->GPR4;

  c->GPRx = fs_read(fd, buf, len);
}

void sys_lseek(Context *c) {
  int fd = (int) c->GPR2;
  size_t offset = (size_t) c->GPR3;
  int whence = (int) c->GPR4;
  
  c->GPRx = fs_lseek(fd, offset, whence);
}

void sys_close(Context *c) {
  int fd = (int) c->GPR2;
  c->GPRx = fs_close(fd);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit : strace(c); sys_exit(c);   break;
    case SYS_yield: strace(c); sys_yield(c);  break;
    case SYS_write:  sys_write(c);  break;
    case SYS_brk:   strace(c); sys_sbrk(c);   break;
    case SYS_open:  strace(c); sys_open(c);strace(c);   break;
    case SYS_read:  strace(c); sys_read(c);   break;
    case SYS_lseek: strace(c); sys_lseek(c);  break;
    case SYS_close: strace(c); sys_close(c);  break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
}
