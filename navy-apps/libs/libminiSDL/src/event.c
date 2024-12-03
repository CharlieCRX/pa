#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

// 按键名称到 SDL_Keys 枚举的映射
#define KEY_TO_ENUM(k) if (strcmp(key_name, #k) == 0) return SDLK_##k;

int get_key_code(const char *key_name) {
  if (key_name == NULL) return SDLK_NONE;

  _KEYS(KEY_TO_ENUM)  // 使用宏生成所有按键的判断

  return SDLK_NONE;  // 如果没有匹配的按键，返回 SDLK_NONE
}
int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(1) {
    char buf[64];
    int result = NDL_PollEvent(buf, sizeof(buf));
    if (result == 0) continue;

    // kd DOWN/kd RIGHT 去除固定字符
    char key_oper[10];
    char key_name[64];
    snprintf(key_oper, 3, "%s", buf);
    printf("SDL_WaitEvent: %s\n", key_oper);
    if (strcmp(key_oper, "ku") == 0) {
      event->type = SDL_KEYUP;
    } else if (strcmp(key_oper, "kd") == 0) {
      event->type = SDL_KEYDOWN;
    }
    sprintf(key_name, "%s", buf+3);
    printf("SDL_WaitEvent: %s\n", key_name);

    event->key.keysym.sym = get_key_code(key_name);

    return 0;
  }

}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
