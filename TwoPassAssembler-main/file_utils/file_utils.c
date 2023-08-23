#include <stdlib.h>
#include <string.h>
#include "../file_utils/file_utils.h"
#include "../globals.h"
#include "../in_utils/input_utils.h"
#include "../management/table_management.h"
#include "../assembler_utils/assembler_utils.h"

int open_file(FILE** file, char* filename, char* filetype, char* io_mode)
{
    /* Concatenate filename and file typee */
    filename = str_concat_ret(filename, filetype);
    if (filename == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    /* Open file according to specified mode */
    *file = fopen(filename, io_mode);
    if (*file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", filename);
        free(filename);
        return 0;
    }
    free(filename);
    return 1;
}

int create_object_file(char* filename, CPU * assembler_cpu, const unsigned int * IC, const unsigned int * DC) {
    int i;
    char * code;
    FILE * ob_dst_file;

    /* Create the object file */
    if (!open_file(&ob_dst_file, filename, ".ob", "w"))
        return 0;

    /* Print head of file containing IC and DC */
    fprintf(ob_dst_file, "%d %d\n", *IC - INSTRUCTION_COUNTER_INITIAL_ADDRESS, *DC);

    /* Copies 64bit-encoded instruction image to object file */
    for (i = INSTRUCTION_COUNTER_INITIAL_ADDRESS; i < (*IC); ++i) {
        code = base64_encode_12bit(assembler_cpu->instruction_image[i].bits);
        fputs(code, ob_dst_file);
        fputs("\n", ob_dst_file);
        free(code);
    }

    /* Copies 64bit-encoded data image to object file */
    for (i = 0; i < (*DC); ++i) {
        code = base64_encode_12bit(assembler_cpu->data_image[i].bits);
        fputs(code, ob_dst_file);
        fputs("\n", ob_dst_file);
        free(code);
    }
    fclose(ob_dst_file);
    return 1;
}

int create_file_for_entry_type(char* filename, symbol_table* symbols, symbol_type type, char* filetype) {
    FILE * ent_dst_file;
    symbol_table* entry_symbols;
    symbol_table* current;
    int size = 0;
    int file_created = 0;

    /* Find all entry type symbols */
    entry_symbols = find_symbols_by_type(symbols, type, &size);

    /* If there are any entry symbols, a .ent file is created */
    if (size > 0) {
        if (!open_file(&ent_dst_file, filename, filetype, "w"))
            return file_created;

        /* Print all entry symbols to file */
        current = entry_symbols;
        while (current != NULL) {
            fprintf(ent_dst_file, "%s\t%d\n", current->name, current->value);
            current = current->next;
        }
        free_symbols(entry_symbols);
        file_created = 1;
        fclose(ent_dst_file);
    }
    return file_created;
}

int create_file_for_extern_type(char* filename, char* filetype, symbol_table * extern_symbols) {
    FILE * ent_dst_file;
    symbol_table *current;
    int file_created = 0;

    /* If there are any entry symbols, a .ext file is created */
    if (extern_symbols != NULL) {
        if (!open_file(&ent_dst_file, filename, filetype, "w"))
            return file_created;

        /* Print all extern symbols to file */
        current = extern_symbols;
        while (current != NULL) {
            fprintf(ent_dst_file, "%s\t%d\n", current->name, current->value);
            current = current->next;
        }
        file_created = 1;
        fclose(ent_dst_file);
    }
    return file_created;
}

int is_fgets_buffer_exceeded(FILE * file, char * str) {
    int c;
    /* If str doesnt contain \n and EOF wasn't reached */
    if (strchr(str, '\n') == NULL && !feof(file)) {
        /* Read file until \n or EOF is found */
        while ((c = getc(file)) != '\n' && c != EOF);
        return 1;
    }
    return 0;
}

int is_fgets_buffer_exceeded_copy(FILE * src_file, FILE * dst_file, char * str) {
    int c;
    /* If str doesnt contain \n and EOF wasn't reached */
    if (strchr(str, '\n') == NULL && !feof(src_file)) {
        /* Read file until \n or EOF is found, while writing the source file content to the output file */
        fputs(str, dst_file);
        while ((c = getc(src_file)) != '\n' && c != EOF) {
            putc((char)c, dst_file);
        }
        putc((char)c, dst_file);
        return 1;
    }
    return 0;
}