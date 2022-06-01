#include <stdio.h>

#define max(A,B) ((A) > (B) ? (A) : (B))
#define square(x) x*x
int main() {
    int i = 0, j = 0;
    int val = square(2-1);
    printf("%d\n", val);
}

void isr_function() {}