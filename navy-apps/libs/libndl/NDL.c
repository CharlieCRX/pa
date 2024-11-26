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
char* get_value(KeyValuePair pairs[], int num_pairs, const char* key);
int get_width(KeyValuePair pairs[]);
int get_height(KeyValuePair pairs[]);
void str_to_pairs(const char *mulKeyValueString, KeyValuePair pairs[]);

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

  // 将键值对字符串解析为结构体数组
  char line[256];  // 用来存储读取的一行内容
  while (fgets(line, sizeof(line), fp) != NULL) {
    // fgets 读取到一行内容
    printf("Read line: %s\n", line);
  }

  
  str_to_pairs(line, pairs);
  *width = get_width(pairs);
  *height = get_height(pairs);
  printf("screen size:width = %d, height = %d\n", *width, *height);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
    int fbctl = 4;
    fbdev = 5;
    // if(w == 0 && h == 0) {
      NDL_GetDisplayInfo(w, h);
    // }
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
  printf("AAAAAAAAAAAAAaaaaaaaaaaaaaaaaaaaaaaaa\n");
  NDL_GetDisplayInfo(w, h);
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
char* get_value(KeyValuePair pairs[], int num_pairs, const char* key) {
  for (int i = 0; i < num_pairs; i++) {
    if (strcmp(pairs[i].key, key) == 0) {
      return pairs[i].value;
    }
  }
  return NULL;  // 如果没找到
}

int get_width(KeyValuePair pairs[]) {
  char width[MAX_VALUE_LEN];
  char *temp = get_value(pairs, 2, "WIDTH");
  assert(strlen(temp) >= MAX_VALUE_LEN);
  strcpy(width, temp);

  int w = -1;
  assert(sscanf(width, "%d", &w) == 1);
  return w;
}

int get_height(KeyValuePair pairs[]) {
  char height[100];
  char *temp = get_value(pairs, 2, "HEIGHT");
  assert(strlen(temp) >= MAX_VALUE_LEN);
  strcpy(height, temp);

  int h = -1;
  assert(sscanf(height, "%d", &h) == 1);
  return h;
}

/**
 * @brief 将一个键值对字符串转换为键值对结构体
 * 字符串格式："key1:'abc'" 或者 "key1: 123"
 * @param oneKeyValueString 包含一个键值对的字符串
 * @param pair 一个键值对结构体
 * @date 2024-11-26
 */
void str_to_one_pair(const char *oneKeyValueString, KeyValuePair *pair) {
  // 复制字符串避免被修改
  char tempStr[30];
  assert(strlen(oneKeyValueString) <= 30);
  strcpy(tempStr, oneKeyValueString);

  // 将字符串按照键值之间的分隔符 ':' 分割
  // 获取冒号的位置
  int i = 0;
  while(tempStr[i] != ':') {
    i++;
  }
  assert(i > 0);

  // 未处理字符中的空格
  strncpy(pair->key, tempStr, i);
  pair->key[i] = '\0';
  strncpy(pair->value, tempStr + i + 1, strlen(oneKeyValueString) - i);
  pair->value[strlen(oneKeyValueString) - i] = '\0';
}

/**
 * @brief 将包含多个key-value的字符串转换为键值对结构体数组
 * 字符串格式为：" key1:123, key2:'23a', key5:'abc' "
 * 处理步骤为：
 *  1. 遍历每一对键值对
 *  2. 将一个键值对元素转换为结构体元素
 * @param keyValueString 包含多个键值对的字符串
 * @param pairs 包含字符串键值对类的结构体数组
 * @date 2024-11-26
 */
void str_to_pairs(const char *mulKeyValueString, KeyValuePair pairs[]) {
  char tempStr[500];
  assert(strlen(mulKeyValueString) <= 500);
  strcpy(tempStr, mulKeyValueString);

  // 将键值对按照分隔符 ","分割
  int i = 0;
  char *token = strtok(tempStr, ",");
  while (token != NULL) {
    // 去除可能的前后空白字符
    while (*token == ' ') token++;

    // 调用解析单个键值对的函数
    str_to_one_pair(token, &pairs[i]);
    i++;

    // 获取下一个键值对
    token = strtok(NULL, ",");
  }
}