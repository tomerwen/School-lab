#include <stdio.h>
#include <stdlib.h>

#define ENLARGE_SIZE realloc(set,5*sizeof(int));
int set[5];
int arraysize = 5;

void print_set() { /* print the set in orginized way*/
    int i;
    for (i = 0; i <= arraysize; i++){
        printf("%d", set[i]);
        if (i < arraysize){
            printf(", ");
        }
    }
}
void get_set() {
    int input;
    int j = 0;

    printf("Please enter your inputs for the set: ");
    while (scanf("%d", &input) == 1) {
        int i;
        for (i = 0; i < j; i++) {
            if (set[i] == input) {
                break;
            }
        }
        if (i == j) {
            set[j++] = input;
            if (j == arraysize) {
                ENLARGE_SIZE;
                arraysize += 5;
            }
        }
    }
}

int main(){ /*run the program*/
    get_set();
    print_set();

}