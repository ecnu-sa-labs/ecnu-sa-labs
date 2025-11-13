#include "Symbolic.h"

int main() {
    int a;
    make_symbolic(&a, sizeof(a), "a");

    __builtin_trap();

    return 0;
}