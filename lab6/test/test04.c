void f() {
  int x = 0;
  int y = 2;
  int z;
  if (x < 1) {
    z = y / x; // divide-by-zero within branch
  }
}
