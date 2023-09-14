#include <stdio.h>

int main() {
  int a = 0;
  int b = 0;
  int c = getchar();
  int d;
  do {
    a = 1;
    b = c;
    if (c < 50) {
      a = 0;
      c++;
    }
  } while (b != c);
  d = 1 / a;
  return 0;
}