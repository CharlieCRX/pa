#include "include/fixedptc.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

void test_expr1() {
  fixedpt a = fixedpt_rconst(1.2);
  fixedpt b = fixedpt_fromint(10);
  int c = 0;
  if (b > fixedpt_rconst(7.9)) {
    c = fixedpt_toint(fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3)));
  }
  printf("c = %d\n", c);
}


void test_mul() {
  // a. 1.3 * 2.6 = 3.38
  fixedpt A = fixedpt_rconst(1.3);  // 0x14D
  fixedpt B = fixedpt_rconst(2.6);  // 0x29A
  assert(A == 0x14d && B == 0x29A);
  fixedpt result1 = fixedpt_mul(A, B); // 0x14D * 0x29A / 0x100 = 0x362 --> 3.3828125
  assert(result1 == 0x362);

  // b. 6.27 * 3.15 = 19.7505
  A = fixedpt_rconst(6.27); // 0x645
  B = fixedpt_rconst(3.15); // 0x326
  assert(A == 0x645 && B == 0x326);
  fixedpt result2 = fixedpt_mul(A, B); // 0x13BD == 19.738
  assert(result2 == 0x13BD);

  // c. -1.325 * 4.12 = -5.459
  A = fixedpt_rconst(-1.325); // -339 -> FFFFFEAD
  B = fixedpt_rconst(4.12);   // 1055 -> 41f
  assert(A == 0xFFFFFEAD && B == 0x41f);
  fixedpt result3 = fixedpt_mul(A, B);  // FFFA 8AF3 / 0x100 = FFFFFA8B(-1397) = -5.45703
  assert(result3 == 0xFFFFFA8B);
  // abs Test
  assert(fixedpt_abs(result3) == 0x575); // 0x575 = 1397 --> 5.457 << 8

  // d. 1.42 * 2 = 2.84
  A = fixedpt_rconst(1.42);  // 364 . 16c
  B = fixedpt_fromint(2);  // 512 . 200
  fixedpt result4 = fixedpt_mul(A, B);  // 2D8 == 2.843
  assert(result4 == 0x2D8);
  fixedpt result5 = fixedpt_muli(A, 2);
  assert(result5 == 0x2D8);
  printf("test_mul OK!\n");
}

void test_div() {
  // a. real: 7.59 / (-1.32) = -5.75
  //    fix :-5.75 * 1 << 8 = -1472 = 0xFFFFFA40
  //    abs(real) = abs(-5.75) = 5.75
  //    abs(fix) = 5.75 * 256 = 1472 = 0x5C0
  fixedpt A = fixedpt_rconst(7.59); //   fix: 1943 -> 0x797
  fixedpt B = fixedpt_rconst(-1.32); //  fix: -338 -> 0xFFFFFEAE
  assert(A == 0x797 && B == 0xFFFFFEAE);
  
  //  (现实状态) 0x797 * 0x100 / 0xFFFFFEAE  = -5.746 * 256 = -1471 = 0xFFFFFA41
  //  (理想状态) A / B = (a / b) * 1 << 8    = -5.750 * 256 = -1472 = 0xFFFFFA40
  fixedpt div_result = fixedpt_div(A, B); 
  assert(div_result == 0xFFFFFA41);

  // real: -1.32 / 2 = -0.66 -> fixdept: -169 = 0xFFFFFF57
  assert(fixedpt_divi(B, 2) == 0xFFFFFF57); 

  printf("test_div OK!\n");
}

void test_abs() {
  /**
   * @brief 1. 正实数测试
   * 
   * real   : a = 12.78 , abs(a) = 12.78
   * fixedpt: A = 0xCC8   abs(A) = 0xCC8
   * @date 2024-11-28
   */
  fixedpt A = fixedpt_rconst(12.78);
  assert(A == 0xCC8);
  fixedpt absA = fixedpt_abs(A);
  assert(absA == 0xCC8 && A == absA);


  /**
   * @brief 2. 负实数测试
   * 
   * real   : a = -12.78,       abs(a) = 12.78
   * fixdept: A = 0xFFFFF338    abs(A) = 0xCC8
   * @date 2024-11-28
   */
  A = fixedpt_rconst(-12.78);
  assert(A == 0xFFFFF338);
  absA = fixedpt_abs(A);
  assert(absA == 0xCC8 && A == -absA);
  printf("test_abs OK!\n");
}


void test_floor() {
  assert(floor(12.78) == 12);
  assert(floor(-12.78) == -13);
  /**
   * @brief 1. 正数测试
   * real    : a = 12.78  floor(a) = 12
   * fixedpt : A = 0xCC8  floor(A) = 0xC00
   * @date 2024-11-28
   */
  fixedpt A = fixedpt_rconst(12.78);
  assert(A == 0xCC8);
  assert(0XC00 == fixedpt_floor(A));

  /**
   * @brief 2. 负数测试
   * 
   * case1: 带小数
   * real   : b = -12.78,     floor(b) = -13
   * fixedpt: B = 0xFFFFF338, floor(B) = 0xFFFFF300
   * floor(B) = B >> 8 << 8
   * 
   * case2: 不带小数
   * real   : b = -12,        floor(b) = -12
   * fixedpt: B = 0xFFFFF400, floor(B) = 0xFFFFF400
   * floor(B) = B >> 8 << 8
   * 
   * 可以发现两者floor操作都是清空小数部分即可
   * @date 2024-11-28
   */
  fixedpt B = fixedpt_rconst(-12.78);
  assert(B == 0xFFFFF338);
  assert(0xFFFFF300 == fixedpt_floor(B));
  assert(0xFFFFF300 == fixedpt_rconst(-13));
  assert(0xFFFFF400 == fixedpt_rconst(-12));

  fixedpt C = fixedpt_fromint(-12); // 0xFFFFF400
  assert(C == 0xFFFFF400);
  assert(fixedpt_floor(C) == 0xFFFFF400);
  printf("test_floor OK!\n");
}


void test_ceil() {
  assert(ceil(12.78) == 13);
  assert(ceil(12.01) == 13);
  assert(ceil(12) == 12);
  /**
   * @brief 正数测试
   * case 1: 带小数   --> 整数部分+1，清空小数部分
   * real   : a = 12.78,   ceil(a) = 13
   * fixedpt: A = 0xCC8,   ceil(A) = 0xD00
   * 
   * real   : b = 12.01,   ceil(B) = 13
   * fixedpt: B = 0xC03,   ceil(B) = 0xD00
   * 
   * case 2: 不带小数  --> 无特殊操作
   * real   : c = 12,      ceil(c) = 12
   * fixedpt: C = 0xC00,   ceil(C) = 0xC00
   * @date 2024-11-28
   */
  fixedpt A = fixedpt_rconst(12.78);
  fixedpt B = fixedpt_rconst(12.01);
  fixedpt C = fixedpt_rconst(12);
  assert(A == 0xCC8);
  assert(fixedpt_fracpart(A) == 0xC8);
  assert((A - fixedpt_fracpart(A) + FIXEDPT_ONE) == 0xD00);
  assert(fixedpt_ceil(A) == 0xD00);
  assert(fixedpt_ceil(B) == 0xD00);
  assert(fixedpt_ceil(C) == 0xC00);



  assert(ceil(-0.5) == 0);
  assert(ceil(-12.78) == -12);
  assert(ceil(-5) == -5);
  /**
   * @brief 负数测试
   * case1: 带小数  --> 整数部分+1，清空小数部分
   * real   : a = -0.5,       ceil(a) = 0
   * fixedpt: A = 0xFFFFFF80, ceil(A) = 0
   * 
   * real   : b = -12.78,     ceil(b) = -12
   * fixedpt: B = 0xFFFFF338, ceil(b) = 0XFFFFF400
   * 
   * case2: 不带小数  --> 无特殊操作
   * real   : c = -5,         ceil(c) = -5
   * fixedpt: c = 0XFFFFFB00, ceil(C) = 0XFFFFFB00
   * 
   * @date 2024-11-28
   */
  A = fixedpt_rconst(-0.5);
  B = fixedpt_rconst(-12.78);
  C = fixedpt_rconst(-5);
  assert(fixedpt_ceil(A) == 0);
  assert(fixedpt_ceil(B) == 0XFFFFF400);
  assert(fixedpt_ceil(C) == 0XFFFFFB00);
  printf("test_ceil OK!\n");
}


int main() {
  test_mul();
  test_div();
  test_abs();
  test_floor();
  test_ceil();
  return 0;
}