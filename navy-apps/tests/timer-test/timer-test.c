#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  struct timeval start, now;
  long elapsed_time_us; // 用于计算已经过去的时间
  const long interval_us = 500000; // 0.5秒对应的微秒数

  // 获取开始时间
  gettimeofday(&start, NULL);

  while (1) {
    gettimeofday(&now, NULL);

    // 计算经过的时间 (秒 + 微秒)
    elapsed_time_us = (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);

    if (elapsed_time_us >= interval_us) {
        printf("This is a line of text.\n");

        // 更新开始时间，避免累积误差
        start = now;
    }
  }

  return 0;
}
