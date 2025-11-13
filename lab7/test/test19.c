#include <stdio.h>

int main() {
  int stop = getchar();

  if (stop > 4) {
    stop = 2;
  }

  int *point = NULL;

  int a = 1;
  int b = 2;
  int c = 0;
  int d = 3;

  if (stop == 0) {
    point = &a;
  } else if (stop == 1) {
    point = &b;
  } else if (stop == 2) {
    point = &c;
  } else {
    point = &d;
  }

  int *e = &a;
  int *f = &a;

  int y = *f / *point;
  return 0;
}
