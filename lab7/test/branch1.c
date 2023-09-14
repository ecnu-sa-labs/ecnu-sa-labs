#include <stdio.h>

#include "../include/Runtime.h"

int main() {
  int x;
  DSE_Input(x);
  int y;
  DSE_Input(y);
  int z;
  DSE_Input(z);

  if (x == y && y == z) {
    x = x / (y - z);
  }

  return 0;
}
