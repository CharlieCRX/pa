#include <common.h>
extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

size_t ramdisk_read(void *buf, size_t offset, size_t len);
/* write `len' bytes starting from `buf' into the `offset' of ramdisk */
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
void init_ramdisk();
size_t get_ramdisk_size();