#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define temp(k) SDLK_##k

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

    event->key.keysym.sym = temp(key_name);

    return 0;
  }

}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
