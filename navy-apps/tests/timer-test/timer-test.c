#include <stdio.h>
#include <unistd.h>
#include <NDL.h>


int main() {
  uint32_t start, now, elapsed_time_us; // 用于计算已经过去的时间
  const uint32_t interval_us = 500; // 0.5秒对应的毫秒数

  NDL_Init(0);
  // 获取开始时间
  start = NDL_GetTicks();

  while (1) {
    uint32_t now = NDL_GetTicks();
    // 计算经过的时间 (毫秒)
    elapsed_time_us = (now - start) * 1000;  
    if (elapsed_time_us >= interval_us) {
      printf("This is a line of text.\n");
      // 更新开始时间，避免累积误差
      start = now;
    }
  }

  return 0;
}
