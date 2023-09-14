#include <stdio.h>
#include <string.h>

int main() {
  char input[65536];
  fgets(input, sizeof(input), stdin);
  int x = 0;
  int y = 2;
  int z;
  if (strlen(input) > 50) {
    if (strlen(input) > 60) {
      if (strlen(input) > 70) {
        if (strlen(input) > 80) {
          if (strlen(input) > 90) {
            if (strlen(input) > 100) {
              if (strlen(input) > 110) {
                if (strlen(input) > 120) {
                  if (strlen(input) > 130) {
                    if (strlen(input) > 140) {
                      z = y / x;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
