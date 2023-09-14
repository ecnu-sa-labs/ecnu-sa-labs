#include <stdio.h>
#include <string.h>

int main() {
  int x, y, z;
  char buf0[6000], buf1[6000], buf2[6000];
  int a = 0;
  int r = 43;
  fgets(buf0, sizeof(buf0), stdin);
  fgets(buf1, sizeof(buf1), stdin);
  fgets(buf2, sizeof(buf2), stdin);
  int b = 0;
  printf("%d, %d, %d\n", strlen(buf0), strlen(buf1), strlen(buf2));
  if (strlen(buf0) < 100)
    if (strlen(buf0) > 10)
      if (strlen(buf0) < 70)
        if (strlen(buf0) > 20)
          if (strlen(buf1) < 250)
            if (strlen(buf1) < 200)
              if (strlen(buf1) > 20)
                if (strlen(buf1) > 30)
                  if (strlen(buf2) < 350)
                    if (strlen(buf2) < 300)
                      if (strlen(buf2) > 100)
                        if (strlen(buf2) > 120)
                          b = 1;
  if (b)
    printf("%d\n", r / a);
  return 0;
}
