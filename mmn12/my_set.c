#include <stdio.h>
#include <stdlib.h>

#define ENLARGE_SIZE realloc(set,5*sizeof(int));


void print_set(int set[], int size){ /* print the set in orginized way*/
    int i;
    for(i=0;i<size;i++){
        printf("%d", set[i]);
    }
}
void get_set(){ /* recieve input from user and build group*/
    int arraysize = 5;
    int input=0;
    int j=0;
    int set[5];
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
            ENLARGE_SIZE;
            arraysize=arraysize+5;
        }
        if(set[i]==input){
            break;
        }
        set[j]=input;
        j++;
    }
    }
    print_set(set,arraysize);
}


int main(){ /*run the program*/
    get_set();
}