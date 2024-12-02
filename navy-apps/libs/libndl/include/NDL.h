#ifndef __NDL_H__
#define __NDL_H__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int NDL_Init(uint32_t flags);
void NDL_Quit();
uint32_t NDL_GetTicks();
void NDL_OpenCanvas(int *w, int *h);
int NDL_PollEvent(char *buf, int len);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);
void NDL_OpenAudio(int freq, int channels, int samples);
void NDL_CloseAudio();
int NDL_PlayAudio(void *buf, int len);
int NDL_QueryAudio();



#define ANSI_NONE       "\33[0m"
#define ANSI_FG_RED     "\33[1;31m"

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE
#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      printf(ANSI_FMT(format, ANSI_FG_RED) "\n", ## __VA_ARGS__); \
      assert(cond); \
    } \
  } while (0)

#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#define TODO() panic("please implement me")

#ifdef __cplusplus
}
#endif

#endif
