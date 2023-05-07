#include <stdio.h>
#include <stdlib.h>

#define ENLARGE_SIZE set = realloc(set, (arraysize + 5) * sizeof(int))
int *set;
int arraysize = 5;

void print_set() {
    printf("This is the set: ");
    for (int i = 0; i < arraysize; i++) {
        printf("%d", set[i]);
        if (i < arraysize - 1) {
            printf(", ");
        }
    }
    printf("\n");
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
}

int main() {
    set = malloc(arraysize * sizeof(int));
    if (set == NULL) {
        printf("Memory allocation error\n");
        return 1;
    }
    get_set();
    print_set();
    free(set);
    return 0;
}
