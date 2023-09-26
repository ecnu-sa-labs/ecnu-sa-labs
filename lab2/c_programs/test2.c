#include <stdio.h>

int main() {
  int a = getchar();
  int b = 1;
  int d = 0;
  if (a == 0) {
    a = (b >= 0);
    d = b / a;
  }
  return 0;
}
