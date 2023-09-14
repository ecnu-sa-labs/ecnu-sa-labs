void f() {
  int x = 0;
  int y = 2;
  int z;
  if (x < 1) {
    z = x / y;
  } else {
    z = z / x;
  }
}

int main() {
  f();
}
