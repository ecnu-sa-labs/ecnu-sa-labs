#include <stdio.h>

void f() {
  int in = getchar();
  unsigned int a = 10;
  unsigned int b = 2;

  if (in > 0) {
    b = 0;
  } else if (in == 0) {
    b = 2 - b;
  } else {
    b = -2 + b;
  }

  int out = a / b;
}
