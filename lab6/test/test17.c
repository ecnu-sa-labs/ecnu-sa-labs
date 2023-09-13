#include <stdio.h>

int main() {
  int a = 0;
  int b = 1;
  int *p = &a;
  int *q = &b;

  *p = *q;

  int s = b / *p;
  return 0;
}
