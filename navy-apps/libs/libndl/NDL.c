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
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100

// 定义结构体表示键值对
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

const char* get_value(KeyValuePair pairs[], int num_pairs, const char* key);

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
  assert(evtdev != -1);//检查是否init
  // 判断events文件是否打开
  FILE *fp = fopen("/dev/events", "r");
  assert(fp);

  //从fp所指的文件中，读取字长为len的数据
  if(fgets(buf, len, fp) == NULL) {
    return 0;
  }
  return strlen(buf);
}

/**
 * @brief 获取屏幕信息
 * `/proc/dispinfo`文件的格式："WIDTH:400, HEIGHT:300"
 * @param width 
 * @param height 
 * @date 2024-11-25
 */
void NDL_GetDisplayInfo(int *width, int *height) {
  FILE *fp = fopen("/proc/dispinfo", "r");
  assert(fp);

  // 建立两个键值对存放高度和宽度
  KeyValuePair pairs[2];

  // 将键值对转换为对应的值

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
  evtdev = 0;
  return 0;
}

void NDL_Quit() {
  now_time = -1;
}


//helper func
/**
 * @brief Get the value object
 * 
 * @param pairs 
 * @param num_pairs 
 * @param key 
 * @return const char* 
 * @date 2024-11-25
 */
const char* get_value(KeyValuePair pairs[], int num_pairs, const char* key) {
  for (int i = 0; i < num_pairs; i++) {
    if (strcmp(pairs[i].key, key) == 0) {
      return pairs[i].value;
    }
  }
  return NULL;  // 如果没找到
}

int get_width(KeyValuePair pairs[]) {
  const char width[100] = get_value(pairs, 2, "WIDTH");
  int w = -1;
  assert(sscanf(width, "%d", &w) == 1);
  return w;
}

int get_height(KeyValuePair pairs[]) {
  const char height[100] = get_value(pairs, 2, "HEIGHT");
  int h = -1;
  assert(sscanf(height, "%d", &h) == 1);
  return h;
}

// 字符串转换为键值对
int parse_key_value(const char *str, KeyValuePair *pair) {
  char temp_str[MAX_KEY_LEN + MAX_VALUE_LEN + 1];
  strncpy(temp_str, str, sizeof(temp_str) - 1);
  temp_str[sizeof(temp_str) - 1] = '\0';

  char *key = strtok(temp_str, "=");
  char *value = strtok(NULL, "=");

  if (key && value) {
    strncpy(pair->key, key, MAX_KEY_LEN);
    strncpy(pair->value, value, MAX_VALUE_LEN);
    return 0; // 成功解析
  }
  return -1; // 解析失败
}

// 将多个键值对字符串转换为键值对数组
int parse_multiple_key_values(const char *strcmp, KeyValuePair pairs[], int max_pairs) {
  // 获取字符串中的键值对数量

}