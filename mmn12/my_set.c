#include <stdio.h>

#define ENLARGE_SIZE realloc(set,5*sizeof(int));
/* needs to be used on realloc function*/
void get_set(){ /* recieve input from user and build group*/
    int arraysize = 5;
    printf("Please enter your inputs for the set");
    int input;
    int j=0;
    int *set[5];
    while(input!=EOF){
    scanf("%d",&input);
    int i;
    for(i=0;i<=j;i++){
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
        set[j+1]==input;
    }
    }
    print_set(set,arraysize);
}

void print_set(int set[], int size){ /* print the set in orginized way*/
    int i;
    for(i=0;i<size;i++){
        printf("%d", set[i]);
    }
}

int main(){ /*run the program*/
printf("Please enter your list of integers");

    get_set();
}