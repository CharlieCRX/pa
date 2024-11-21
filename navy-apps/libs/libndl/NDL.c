#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static uint64_t now_time = -1;

// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
  if(now_time == -1) {
    assert(0);//目前配合init的工作
  }
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

/**
 * @brief 读出一条按键事件信息, 将其写入`buf`中, 最长写入`len`字节
 * @param buf 
 * @param len 
 * @return int 读出了有效的事件, 返回1, 否则返回0
 * @date 2024-11-21
 */
int NDL_PollEvent(char *buf, int len) {
  assert(evtdev != -1);
  // 判断events文件是否打开
  FILE *fp = fopen("/dev/events", "r+");
  assert(fp);
  
  //读取数据
  fscanf(fp, "%s", buf);
  return strlen(buf);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  now_time = 0;
  return 0;
}

void NDL_Quit() {
  now_time = -1;
}
