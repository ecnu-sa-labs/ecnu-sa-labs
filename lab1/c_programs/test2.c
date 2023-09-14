#include <stdio.h>

int main() {
  int k = 0;
  int c = (getchar() - 'W') % 10;
  for (int i = 0; i < c; i++) {
    for (int j = i; j < c; j++) {
      k += 100 / (j - i);
    }
  }
  return 0;
}