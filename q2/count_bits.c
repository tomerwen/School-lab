#include <stdio.h>

void count_bits(unsigned long n){
int i;
    unsigned long mask = 1ul << (sizeof(unsigned long) * 8 - 1);
    int even_bits_count=0;
    for(i=0;i<sizeof(unsigned long)*8;i++){
        putchar((n & mask) ? '1':'0');
        if(i % 2 == 0 && (n & mask) && i!=0){
            even_bits_count++;
        }
        n<<=1;
    }
    putchar('\n');
    printf("There are %d even bits that are on.\n" , even_bits_count);
}

int main(){
    unsigned long n;
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