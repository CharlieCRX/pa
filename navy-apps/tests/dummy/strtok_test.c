#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_KEY_LEN 20  // key最大长度
#define MAX_VAL_LEN 20  // value最大长度
#define MAX_NUMS 10     // 最多10个键值对
//创建键值对结构体
typedef struct 
{
  char key[MAX_KEY_LEN];
  char value[MAX_VAL_LEN];
}keyValuePair;


/**
 * @brief 将一个键值对字符串转换为键值对结构体
 * 字符串格式："key1:'abc'" 或者 "key1: 123"
 * @param oneKeyValueString 包含一个键值对的字符串
 * @param pair 一个键值对结构体
 * @date 2024-11-26
 */
void str_to_one_pair(const char *oneKeyValueString, keyValuePair *pair) {
  // 复制字符串避免被修改
  char tempStr[30];
  assert(strlen(oneKeyValueString) < 30);
  strcpy(tempStr, oneKeyValueString);

  // 将字符串按照键值之间的分隔符 ':' 分割
  // 获取冒号的位置
  int i = 0;
  while(tempStr[i] != ':') {
    i++;
  }
  assert(i > 0);

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
void str_to_pairs(const char *mulKeyValueString, keyValuePair pairs[]) {
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

void print_pairs(keyValuePair pairs[], int count) {
  for (int i = 0; i < count; i++) {
    printf("%s:%s\n", pairs[i].key, pairs[i].value);
  }
}


char* get_value(keyValuePair pairs[], int num_pairs, const char* key) {
  for (int i = 0; i < num_pairs; i++) {
    if (strcmp(pairs[i].key, key) == 0) {
      return pairs[i].value;
    }
  }
  assert(0);
}

int get_width(keyValuePair pairs[]) {
  char width[MAX_VAL_LEN];
  char *temp = get_value(pairs, 5, "WIDTH");
  assert(strlen(temp) <= MAX_VAL_LEN);
  strcpy(width, temp);

  int w = -1;
  assert(sscanf(width, "%d", &w) == 1);
  return w;
}

int get_height(keyValuePair pairs[]) {
  char height[100];
  char *temp = get_value(pairs, 5, "HEIGHT");
  assert(strlen(temp) <= MAX_VAL_LEN);
  strcpy(height, temp);

  int h = -1;
  assert(sscanf(height, "%d", &h) == 1);
  return h;
}



void test_get_value() {
  // 定义一个包含常量结构体元素的常量数组
  keyValuePair kvpArray[] = {
    {"key1", "value1"},
    {"key2", "value2"},
    {"key3", "value3"},
    {"HEIGHT", "12345"},
    {"WIDTH", "98765"}
  };
  int value;
  value = get_height(kvpArray);
  assert(value == 12345);

  value = get_width(kvpArray);
  assert(value == 98765);
  //


  printf("Test get value is OK\n");
}


void test_one_pair() {
  char str[] = "TOKEN1111111:mmp";
  keyValuePair pair;
  str_to_one_pair(str, &pair);
  print_pairs(&pair, 1);
}

void test_mul_pairs() {
  // char str[] = "TOKEN1:mmp, TOKEN2:jjp";
  // char str[] = "key1:123, key2:'23a', key5:'abc' ";
  char str[50];
  sprintf(str, "WIDTH:%d, HEIGHT:%d", 400, 900);
  keyValuePair pairs[MAX_NUMS];
  str_to_pairs(str, pairs);
  print_pairs(pairs, 3);
}

int main() {
  test_get_value();
  return 0;
}
