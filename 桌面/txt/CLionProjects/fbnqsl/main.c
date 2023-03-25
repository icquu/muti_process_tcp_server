#include <stdio.h>

int main() {
    long a = 0;
    long b = 1;
    long c = a + b;
    long n = 8;
    for (int i = 0; i < n; ++i) {
        if (i == 0) {
            c = 0;
        } else if (i == 1) {
            c = 1;
        } else {
            c = a + b;
            a = b;
            b = c;
        }
    }
    printf("a=%ld,b=%ld,c=%ld\n", a, b, c);
    return 0;
}
