#include <stdio.h>
#include <string.h>

int main() {
  int a = 0;
  char s[20];
  fgets(s, sizeof(s), stdin);
  int l = 0;
  int r = strnlen(s, 20) - 1;
  if (r < 5) {
    a = 1;
  }
  while (l < r) {
    if (s[l++] != s[r--]) {
      a = 1;
    }
  }
  if (s[2] != s[1] + 4) {
    a = 1;
  }
  int b = 1 / a;
  return 0;
}
