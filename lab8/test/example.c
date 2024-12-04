#include "Symbolic.h"

int main() {
    int a;
    make_symbolic(&a, sizeof(a), "a");

    if (a + 2 == 100 - 10) {
        // Trap, a must be 88
        __builtin_trap();
    } else {
        // Should reach, a can be assigned all values that are not 88
    }
    
    return 0;
}