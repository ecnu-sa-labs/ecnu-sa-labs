#include <stdio.h>

int main() {
  char s[2000];
  fgets(s, sizeof(s), stdin);
  int maxP = 1, curP = 1;
  char curChar = s[0];
  for (int i = 1; i < 10; i++) {
    if (s[i] == curChar) {
      curP++;
    } else {
      curChar = s[i];
      curP = 1;
    }
    if (maxP < curP)
      maxP = curP;
  }
  return curP / (maxP - 5);
}
