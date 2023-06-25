#include <stdio.h>

int main(int argc, char *argv[]){
    if (argc < 1){
    printf("Usage: ./script <file1> <file2> ... <fileN>\n");
       return 1;   
    }
    for (int i = 1; i < argc; i++) {
        // Process each file individually
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("Failed to open file: %s\n", argv[i]);
            continue;
    }
    printf("File: %s\n", argv[i]);
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    
    fclose(file);
    }
    return 0;
}