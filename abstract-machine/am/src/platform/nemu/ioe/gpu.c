#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
	int i;
	int w = BITS(inl(VGACTL_ADDR), 31, 16) / 32;  // TODO: get the correct width
	int h = BITS(inl(VGACTL_ADDR), 15, 0)  / 32;  // TODO: get the correct height
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

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // 若`sync`为`true`, 则马上将帧缓冲中的内容同步到屏幕上
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
