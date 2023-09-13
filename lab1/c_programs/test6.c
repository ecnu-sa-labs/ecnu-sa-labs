#include <stdio.h>

int main() {
  int K = getchar();
  char sum[65536];
  fgets(sum, sizeof(sum), stdin);
  for (int i = 1; i < 13; i++) {
    sum[i] = (sum[i - 1] * 5) + 1;
  }
  for (int i = 12; i >= 0; i--) {
    K = K % sum[i];
  }
  return 0;
}
