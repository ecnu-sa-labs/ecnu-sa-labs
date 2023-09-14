int main() {
  int a = 0;
  int b = 0;
  int c = a == b;
  int d = b / c;
  int e = d / d; // divide by zero
  return 0;
}
