#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <sys/time.h>
#include <time.h>

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

  count = fs_write(fd, buf, count);
  c->GPRx = count;
}

void sys_sbrk(Context *c) {
  c->GPRx = 0;
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

/*描述：用于获取系统时间的系统调用
        tv_sec= 秒数，从1970年1月1日 00:00:00 UTC开始的时间（UNIX时间戳）
        tv_usec= 微秒数（0 ~ 999,999）
        tz_minuteswest = 当前时区与 UTC 时间的差值，以分钟为单位
        tz_dsttime = 是否使用夏令时
  返回值：成功时返回 0，失败时返回 -1
*/
void sys_gettimeofday(Context *c) {
  struct timeval  *tv = (struct timeval *) c->GPR2;
  struct timezone *tz = (struct timezone *) c->GPR3;

  if (tv == NULL) {
    c->GPRx = -1; // 返回错误码
    return;
  }
  // io_read(AM_TIMER_UPTIME):AM系统启动时间, 可读出系统启动后的微秒数
  uint64_t usec= io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = usec / 1000000;
  tv->tv_usec = usec % 999999;

  if(tz != NULL) {
    // 如果传入的 tz 不为空，设置默认值
    tz->tz_minuteswest = -480;  //UTC+8（中国标准时间）
    tz->tz_dsttime = 0; // 假设无夏令时
  }

  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit : strace(c); sys_exit(c);   break;
    case SYS_yield: strace(c); sys_yield(c);  break;
    case SYS_write: strace(c); sys_write(c);  break;
    case SYS_brk:   sys_sbrk(c);   break;
    case SYS_open:   sys_open(c);   break;
    case SYS_read:   sys_read(c);   break;
    case SYS_lseek: sys_lseek(c);  break;
    case SYS_close: strace(c); sys_close(c);  break;
    case SYS_gettimeofday:  strace(c); sys_gettimeofday(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
}
