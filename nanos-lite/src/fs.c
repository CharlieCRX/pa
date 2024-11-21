#include <fs.h>
#include "ramdisk.h"
#include <device.h>
//helper
size_t normal_fs_read(int fd, void *buf, size_t len);
size_t normal_fs_write(int fd, const void *buf, size_t len);
static int check_fd(int fd);
void print_file(int fd);
static size_t operation_offset(int fd);
static size_t valid_operation_len(int fd, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset; // 相对于文件开头的偏移量，用于系统调用
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENT};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin" , 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_EVENT]  = {"/dev/events", 0, 0, 0, events_read, invalid_write},
#include "files.h"
};
#define sys_file(fd) file_table[check_fd(fd)]
#define IS_NORMAL_FD(fd) ((fd) != FD_STDOUT && (fd) != FD_STDERR && (fd) != FD_STDOUT)

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

// 名称：按照名称文件查询文件描述符（文件表下标）
// return: 存在，返回文件下标；不存在，直接报错
int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < sizeof(file_table); i++) {
    print_file(i);
    if(strcmp(pathname, file_table[i].name) == 0) {
      return check_fd(i);
    }
  }
  assert(0);
}


/**
 * @brief 从fd所指的文件，读取长度为len字节的数据到buf中
 * 如果读取的是键盘事件 /dev/events，则不会更新文件的打开偏移
 * 读取成功:
 * - 返回读取数据的字数count（count == len），同时相应文件的打开偏移open_offset加上读取的字数；
 * - 如果最总返回的字数count小于要读取的长度len，说明fd文件的数据不够长；
 * 读取失败:
 * - 返回-1--且此时不改变文件的打开偏移open_offset
 * 
 * @param fd 文件描述符
 * @param buf 读取数据的目的地址
 * @param len 想读取的数据长度
 * @return size_t 实际返回的数据长度
 * @author chenruixin (rxchen1999@163.com)
 * @date 2024-11-21
 */
size_t fs_read(int fd, void *buf, size_t len) {
  if(fd == FD_STDOUT || fd == FD_STDERR)  
  assert(0);
  size_t count = -1;
  // 读取普通文件
  if(sys_file(fd).read == NULL) {
    count = normal_fs_read(fd, buf, len);
  } else {  // 读取事件文件
    count = sys_file(fd).read(buf, 0, len);
  }
  return count;
}


/* 描述：从buf的初始值位置读取len个字节的数据，写入到fd所指的文件中
/   返回值：
    1. 成功写入，return_val == len,返回写入的字节数
                return_val <  len,剩余ramdisk空间不足以写入len数据
    2. 写入失败，返回-1
*/
size_t fs_write(int fd, const void *buf, size_t len) {
  size_t count;
  // 判断文件类型为普通
  if(sys_file(fd).write == NULL) {
    count = normal_fs_write(fd, buf, len);
  } else {
    count = sys_file(fd).write(buf, 0, len);
  }
  return count;
}

// 描述：定位fd所指文件的偏移
// 定位后更新文件的open_offset，偏移量不能超过文件的边界
size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence)
  {
    case SEEK_SET:
      assert(sys_file(fd).size >= offset);
      sys_file(fd).open_offset = offset; 
      break;

    case SEEK_CUR:
      assert(sys_file(fd).size - sys_file(fd).open_offset >= offset); 
      sys_file(fd).open_offset += offset; 
      break;
      
    case SEEK_END:
      assert(offset == 0);
      sys_file(fd).open_offset = sys_file(fd).size;
      break;

    default:
      assert(0);
  }
  Log("After fs_lseek:");
  print_file(fd);
  return sys_file(fd).open_offset;
}


int fs_close(int fd) {
  fs_lseek(fd, 0, SEEK_CUR);
  return 0;
}

/*helper */
static int check_fd(int fd) {
  assert(fd >=0 && fd < sizeof(file_table));
  return fd;
} 

void print_file(int fd) {
  Finfo ready_file = sys_file(fd);
  Log("fileName: %s, size = %d, disk_offset = %d, open_offset = %d",
    ready_file.name, ready_file.size, ready_file.disk_offset, ready_file.open_offset);
}

// 获取fd所指文件的操作偏移地址
static size_t operation_offset(int fd){
  return sys_file(fd).disk_offset + sys_file(fd).open_offset; 
}

// 返回fd号文件的合法读取长度
static size_t valid_operation_len(int fd, size_t len) {
  // 根据文件描述符获取文件信息
  Finfo ready_file = sys_file(fd);
  
  // 合法读取长度
  size_t max_offset   = ready_file.disk_offset + ready_file.size;
  size_t start_offset = operation_offset(fd);
  size_t end_offset   = start_offset + len;

  if (end_offset > max_offset || end_offset < start_offset) { // 右读超长(len = 0xffffffff)
    len = max_offset - start_offset;
  }

  return len;
}

/**
 * @brief 读取普通文件的操作
 * 读取成功:
 * - 返回读取数据的字数count（count == len），同时相应文件的打开偏移open_offset加上读取的字数；
 * - 如果最总返回的字数count小于要读取的长度len，说明fd文件的数据不够长而已；
 * 读取失败:
 * - 返回-1--且此时不改变文件的打开偏移open_offset
 * @param fd 文件描述符
 * @param buf 将读出数据存储到新的地址
 * @param len 理想读出字节数
 * @return size_t 实际读出字节数
 * @date 2024-11-21
 */
size_t normal_fs_read(int fd, void *buf, size_t len){
  assert(IS_NORMAL_FD(fd));
  // 从文件中读取合法长度的数据，同时更新此文件的open_offset
  len = valid_operation_len(fd, len);
  size_t count = ramdisk_read(buf, operation_offset(fd), len);

  // 更新此文件的open_offset
  sys_file(fd).open_offset += count;
  return count;
}


/*  描述：写普通文件的操作 - 存储在ramdisk
/   返回值：
    1. 成功写入，return_val == len,返回写入的字节数
                return_val <  len,剩余ramdisk空间不足以写入len数据
    2. 写入失败，返回-1
*/
size_t normal_fs_write(int fd, const void *buf, size_t len) {
  // 禁止写入特殊文件
  assert(IS_NORMAL_FD(fd));
  len = valid_operation_len(fd, len);

  size_t count = ramdisk_write(buf, operation_offset(fd), len);
  // 更新此文件的open_offset
  sys_file(fd).open_offset += count;  
  return count;
}