#include <stdio.h>

int main() {
  int i, a = 0, b = getchar();
  for (i = 10000; i > b; i--)
    printf("%d\n", b * b / (i - b));
  return 0;
}
