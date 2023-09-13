#include <stdio.h>

int main() {
  int in = getchar();
  int a = 0;
  int b = 1;
  int *x;

  if (in < 10) {
    x = &a;
  } else {
    x = &b;
  }

  int s = b / *x;
  return 0;
}
