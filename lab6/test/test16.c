int f(int arg) {
  int a = 1;
  int b = 1;
  int *x;
  int *y;
  if (arg) {
    x = &a;
    y = &b;
  } else {
    x = &b;
    y = &a;
  }
  int *z;
  if (arg) {
    z = x;
  } else {
    z = y;
  }
  int z2 = 1 / *z;
  return 0;
}
