#include <am.h>
#include <nemu.h>
// 同步寄存器内存地址：0xa000104
#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
	int i;
	int w = BITS(inl(VGACTL_ADDR), 31, 16);  // get the correct width
	int h = BITS(inl(VGACTL_ADDR), 15, 0);   // get the correct height
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for (i = 0; i < w * h; i ++) fb[i] = i;
	outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width  = BITS(inl(VGACTL_ADDR), 31, 16), 
    .height = BITS(inl(VGACTL_ADDR), 15, 0),
    .vmemsz = inl(FB_ADDR)
  };
}

/**
 * @brief 从AM帧缓冲控制器中读取图像信息，写入到显存中
 * AM_GPU_FBDRAW_T参数：int x, y; void *pixels; int w, h; bool sync; 
 * @param AM_GPU_FBDRAW_T *ctl 
 * @date 2024-11-25
 */
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  // 获取图像在屏幕的位置信息(x, y) 和图像大小信息(w, h)
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;

  // 如果同步寄存器为false 并且 要没有要输出的图像（宽高任意为0），则不做任何操作
  if (!ctl->sync && (w == 0 || h == 0)) return;

  uint32_t *pixels = ctl->pixels;

  // 将图像的像素信息 pixels，写到显存中
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t screen_w = BITS(inl(VGACTL_ADDR), 31, 16);

  for (int j = y; j < y+h; j++) { // 固定行的高度为j
    for (int i = x; i < x+w; i++) { // 按列遍历此行的每一个像素点
      fb[i + j*screen_w] = pixels[(i-x) + (j - y)*w];
    }
  }
  
  // 若`sync`为`true`, 则马上将帧缓冲中的内容同步到屏幕上
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
