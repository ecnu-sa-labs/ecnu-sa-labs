#include <stdio.h>

void f() {
  int i = getchar();
  int sum = 0;
  while (sum < 50) {
    sum += i;
  }
  int y = sum - 55;
  int z = i / y;
}
