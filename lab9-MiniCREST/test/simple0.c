#include <stdio.h>

#include "../include/Runtime.h"

int main() {
  int x;
  DSE_Input(x);

  int y = x;

  if (y == 1024) {
    int z = 4 / (y - 1024);
  }

  return 0;
}
