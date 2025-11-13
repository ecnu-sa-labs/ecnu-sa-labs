int f() {
  int a = 1;
  int *c = &a;
  int *d = &a;
  *c = 0;
  int x = 1 / *d;
  return x;
}
