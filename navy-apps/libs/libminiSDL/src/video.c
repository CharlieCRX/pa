#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);

  // 如果没有提供源矩形，使用整个源表面
  if (srcrect == NULL) {
    // printf("srcrect == NULL\n");
    srcrect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
    srcrect->x = 0;
    srcrect->y = 0;
    srcrect->w = src->w;
    srcrect->h = src->h;
    //  &((SDL_Rect){0, 0, src->w, src->h});
  }

  // 如果没有提供目标矩形，使用整个目标表面
  if (dstrect == NULL) {
    // printf("dstrect == NULL\n");
    dstrect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
    dstrect->x = 0;
    dstrect->y = 0;
    dstrect->w = 0;
    dstrect->h = 0;
  }
  
  printf("screen src w = %d, h = %d\n", src->w, src->h);
  printf("screen dst w = %d, h = %d\n", dst->w, dst->h);
  printf("srcrect w = %d, h = %d\n", srcrect->w, srcrect->h);
  printf("dstrect x = %d, y = %d\n", dstrect->x, dstrect->y);


  // 计算目标矩形左上角的索引
  int dst_start_idx = dstrect->y * dst->w + dstrect->x;
  // 计算源头矩形左上角的索引
  int src_start_idx = srcrect->y * src->w + srcrect->x;


  // 获取源和目标的像素指针
  uint32_t *src_pixels = (uint32_t *)src->pixels;
  uint32_t *dst_pixels = (uint32_t *)dst->pixels;

  // 获取源矩形像素信息
  printf("SDL_BlitSurface start drawing...\n");
  for (int y = 0; y < srcrect->h; y++) {
    for (int x = 0; x < srcrect->w; x++) {

      int dst_now_idx = dst_start_idx + y * dst->w + x;

      // 如果映射的目标画布位置超过了画布大小，则不做处理
      if (dst_now_idx > dst->w * dst->h) {
        assert(0);
        // continue;
      } 

      int src_now_idx = src_start_idx + y * src->w + x;
      // printf("dst_now_idx = %d, ", dst_now_idx);
      // printf("src_now_idx = %d\n", src_now_idx);

      // 否则正常复制即可
      dst->pixels[dst_now_idx] = src->pixels[src_now_idx];
      printf("src->pixels[src_now_idx] = 0x%x\n", src->pixels[src_now_idx]);
      // dst->pixels[dst_now_idx] = 0;
      // assert(x == 3);
    }
    // printf("\n");
    // if(y == 0) return;
  }
}

// 往画布的指定矩形区域中填充指定的颜色
void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
  if (dstrect == NULL) {  // full Screen
    // 获取屏幕尺寸 screen_w, screen_h
    int screen_w = dst->w;  // 获取宽度
    int screen_h = dst->h;  // 获取高度

    // 检查目标表面是否足够大
    if (screen_w > dst->w || screen_h > dst->h) {
      assert(0);  // 或者采取其他的错误处理方式
    }

    // 获取图像的像素数据指针
    uint32_t *pixels = (uint32_t *)dst->pixels;
    // 填充整个画布的像素为 color
    for (int y = 0; y < screen_h; y++) {
      for (int x = 0; x < screen_w; x++) {
        pixels[y * screen_w + x] = color;
      }
    }

  } else {
    // 如果 dstrect 不为 NULL，填充指定区域
    int x = dstrect->x;
    int y = dstrect->y;
    int w = dstrect->w;
    int h = dstrect->h;

    // 获取图像的像素数据指针
    uint32_t *pixels = (uint32_t *)dst->pixels;
    // 填充指定矩形区域的像素为 color
    for (int j = y; j < y + h; j++) {
      for (int i = x; i < x + w; i++) {
        pixels[j * dst->w + i] = color;
      }  
    }
  }

  printf("SDL_FillRect OK!\n");
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {
  NDL_DrawRect(s->pixels, x, y , w, h);
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
}
