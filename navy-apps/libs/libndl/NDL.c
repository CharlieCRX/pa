#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_x = 0, canvas_y = 0;
static uint32_t init_ticks = 0;

static uint32_t NDL_GetTicks_internal() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// Get the number of milliseconds since NDL library initialization.
uint32_t NDL_GetTicks() {
  if (init_ticks == 0) return 0; // seems NDL has not been init
  uint32_t now_ticks = NDL_GetTicks_internal();
  return now_ticks - init_ticks;
}

int NDL_PollEvent(char *buf, int len) {
  return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
  int dispinfo_fd = open("/proc/dispinfo", O_RDONLY);
  struct { int w, h; } cfg;
  assert(read(dispinfo_fd, &cfg, sizeof(cfg)));
  close(dispinfo_fd);
  screen_w = cfg.w;
  screen_h = cfg.h;
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }

  canvas_x = (screen_w - *w) / 2;
  canvas_y = (screen_h - *h) / 2;

  printf("[NDL_OpenCanvas]: width=%d, height=%d, w=%d, h=%d\n", screen_w, screen_h, *w, *h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  uint32_t *line = pixels;

  y += canvas_y;
  x += canvas_x;
  for (int i = 0; i < h; i++) {
    lseek(fbdev, ((y + i) * screen_w + x) * sizeof(uint32_t), SEEK_SET);
    write(fbdev, line, w * sizeof(uint32_t));
    line += w;

  }
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
  init_ticks = NDL_GetTicks_internal();
  evtdev = open("/dev/events", O_RDONLY);
  assert(evtdev);
  fbdev = open("/dev/fb", O_WRONLY);
  assert(fbdev);
  return 0;
}

void NDL_Quit() {
  init_ticks = 0;
  close(evtdev);
  close(fbdev);
}
