int f(int arg) {
  int a = 0;
  int b = 1;
  int *x;
  if (arg) {
    x = &a;
  } else {
    x = &b;
  }
  int z = 1 / *x;
  return 0;
}
