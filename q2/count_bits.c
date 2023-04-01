#include <stdio.h>

int count_bits(unsigned long n){
    int i = 0, ret = 0;
    unsigned long x = 0;
    for (i = 0; i < 64; i ++){
        x = n & 1;
        if (x){
            ret++;
        }
        n >>= 2;
    }
    return ret;
}
int main()
{
    unsigned long x;
    printf("-----------------------------------------------------------------------------------");
    printf("Enter an unsigned long integer: \n");
    while(1){
            if(scanf("%lu", &n)==1){
            printf("The bits of %lu are:\n", n);
            count_bits(n);
            } else{
                printf("End of input.");
                break;
                }
            }
            return 0;
}