#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
typedef struct {
  union {
    uint32_t val;
  } inst;
} ISADecodeInfo;



typedef struct Decode {
  uint32_t pc;
  uint32_t dnpc; // dynamic next pc
  ISADecodeInfo isa;
} Decode;


void init_decode(Decode *s, uint32_t pc, uint32_t dnpc, int val);
