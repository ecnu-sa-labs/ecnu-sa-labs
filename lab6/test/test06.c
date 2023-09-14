#include <stdio.h>

void f() {
  int x = getchar();
  int y = 2;
  if (x > 10) {
    y = 0;
  }
  int z = x / y; // divide-by-zero after branch
}
