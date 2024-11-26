#include <common.h>
#include <device.h>
#include <debug.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  putn(buf, len);
  return len;
}

/**
 * @brief 
 * 将键盘事件写入到buf中, 最长写入len字节, 然后返回写入的实际长度
 * 这里定义了两种事件：
 * - 按下按键事件, 如kd RETURN表示按下回车键
 * - 松开按键事件, 如ku A表示松开A键
 * 两种格式要求对IOE读取的键盘事件做相应的转换
 * 
 * @param buf 事件写入地址
 * @param offset 暂时不用
 * @param len 期待字长
 * @return size_t 实际字长， 若当前没有有效按键, 则返回0即可.
 */
size_t events_read(void *buf, size_t offset, size_t len) {
  while(1) {
    // 从IOE的部分读取键盘数据寄存器
    AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);

    // 如果没有有效按键，则返回0
    if (ev.keycode == AM_KEY_NONE) return 0;
    // 将IOE的键盘事件转换为 /dev/events 文件支持的格式
    // 类似"kd RETURN" or "ku A" 
    const char *event_name  = keyname[ev.keycode];
    char *event_keydown = ev.keydown ? "kd" : "ku";

    sprintf(buf, "%s %s", event_keydown, event_name);
    // 获取写入的长度
    break;
  }
  return strlen(buf);
}

/**
 * @brief 获取VGA中关于屏幕的信息
 *  将屏幕信息转换为key-value的形式
 *  这里将key-value转换为结构体：
 *  typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
  } KeyValuePair;
  即"WIDTH:400, HEIGHT:300"

 * @param buf 
 * @param offset 
 * @param len 
 * @return size_t 
 * @date 2024-11-25
 */
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  // 从IOE中读取屏幕信息
  int width  = io_read(AM_GPU_CONFIG).width;
  int height = io_read(AM_GPU_CONFIG).height;

  // 将信息按照key-value的形式存储到buf中
  sprintf(buf, "WIDTH:%d, HEIGHT:%d\n", width, height);

  // 返回输入到buf的键值对字节数
  return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
