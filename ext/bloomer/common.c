#include "common.h"

int comparison(const void* i1, const void* i2) {  
  return *(const int*)i1 - *(const int*)i2;
}
