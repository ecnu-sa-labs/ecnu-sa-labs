#include<stdio.h>

void f() {
  int x = 0, y, z;
  scanf("%d%d", &y, &x);
  if (y > 10)
    y = 0;
  else
    y = 1;
  if (x > 1) {
    z = y / x;
  }
}

int main() {
  f();
}
