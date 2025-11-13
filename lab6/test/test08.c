#include <stdio.h>

void f() {
  int in = getchar();
  int a = 10;
  int b = 2;

  if (in > 0) {
    b = 100 + b;
  } else if (in == 0) {
    b = 1;
  } else {
    b = a + b;
  }

  int out = a / b;
}
