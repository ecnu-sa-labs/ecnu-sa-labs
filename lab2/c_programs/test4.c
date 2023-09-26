#include <stdio.h>

int main() {
  int a = getchar();
  if (a == 1) {
    a = getchar();
    if (a == 3) {
      a = getchar();
      if (a == 5) {
        a = getchar();
        if (a == 7) {
          a = 1 / a;
        }
      }
    }
  }
  return 0;
}
