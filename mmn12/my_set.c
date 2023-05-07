#include <stdio.h>
#include <stdlib.h>

#define ENLARGE_SIZE set=realloc(set,5*sizeof(int));
int *set;
int arraysize = 5;

void print_set() { /* print the set in organized way*/
    int i;
    printf("This is the set: ");
    for (i = 0; i < arraysize; i++){
        printf("i= %i", i);
        printf("%d", set[i]);
        if (i < arraysize - 1){
            printf(", ");
        }
    }
}

void get_set() {
    int input;
    int j = 0;

    printf("Please enter your inputs for the set: \n");
    while (scanf("%d", &input) == 1) {
        int i;
        for (i = 0; i < j; i++) {
            if (set[i] == input) {
                break;
            }
        }
        if (i == j) {
            if (j == arraysize) {
                ENLARGE_SIZE;
                arraysize += 5;
            }
            set[j++] = input;
        }
    }
    set = realloc(set, j * sizeof(int));
    arraysize = j;
}

int main(){ /*run the program*/
    set = malloc(arraysize * sizeof(int));
    get_set();
    print_set();
    free(set);
    return 0;
}
