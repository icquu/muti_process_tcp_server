#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

typedef struct bit {
    unsigned long b:64;
} Bit;

int main() {
    printf("%d\n", sizeof(Bit));
    int flag = 1;
    Bit bit;
    bit.b = 1;
    bit.b = pow(2,64);
    if (errno>0){
        perror("<<");
        exit(1);
    }
    int ret = flag << 3;
    printf("%X\n", ret);
    printf("%lX\n", bit.b);
    return 0;
}
