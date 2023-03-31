#include <stdio.h>

void count_bits(unsigned long n){
int i;
    unsigned long mask = 1ul << (sizeof(unsigned long) * 8 - 1);
    for(i=0;i<sizeof(unsigned long)*8;i++){
        putchar((n & mask) ? '1':'0');
        n<<=1;
    }
    putchar('\n');
}

int main(){
    unsigned long n;
    printf("Enter an unsigned long integer: ");
    scanf("%lu", &n);
    printf("The bits of %lu are:\n", n);
    count_bits(n);
    return 0;
}
