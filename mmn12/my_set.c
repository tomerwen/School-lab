#include <stdio.h>
#include <stdlib.h>

#define ENLARGE_SIZE realloc(set,5*sizeof(int));
int set[5];
int arraysize = 5;

void print_set(){ /* print the set in orginized way*/
    int i;
    for(i=0;i<=arraysize;i++){
        printf("%d", set[i]);
        if(i<arraysize){
        printf(", ");
        }
        }
}
void get_set(){ /* recieve input from user and build group*/
    int input=0;
    int j=0;

    printf("Please enter your inputs for the set");
    while(input!=EOF){
    int i;
    scanf("%d",&input);
    for(i=0;i<=j;i++){
        printf("%d",i);
        if(j==0){
            set[0]=input;
            j++;
        }
        if (j==arraysize){
            set = ENLARGE_SIZE;
            arraysize=arraysize+5;
        }
        if(set[i]==input){
            break;
        }
        set[j]=input;
        j++;
    }
    }
}


int main(){ /*run the program*/
    get_set();
    print_set();

}