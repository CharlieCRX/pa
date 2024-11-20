#include <stdio.h>
#include <assert.h>

int main() {
  // 判断num文件是否打开
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);

  // 判断文件大小
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);

  // 检查文件501到1000行的数据
  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  printf("check 501-1000 lines OK!\n");

  // 修改1到500行的数据
  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }
  printf("modify 1-500 lines data OK!\n");

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
