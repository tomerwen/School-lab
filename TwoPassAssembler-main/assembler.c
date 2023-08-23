#include <stdint.h>
#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_utils.h"

/**
 * Proccesses .as files and converts them to object, external and entry files.
 * @param filename - filename of the fille containing the assembly program.
 * @return 1 if assembler process was successful, else 1.
 */
 int assembler(char * filename);

/* Main program entry point */
int main(int argc, char *argv[]) {
    int i;
    if (argc < 2) { /* Check if there are enough command line arguments */
        printf("Invalid number of arguments.\n");
        return 1; /* Return an error code */
    }

    /* Loop over each command line argument, skipping the program name itself (argv[0]) */
    for (i = 1; i < argc; ++i) {
        assembler(argv[i]); /* Call assembler function for each filename provided as argument */
    }
    return 0;
}

/* Main assembler function */
int assembler(char * filename) {
    /* Initialise counters and flags */
    unsigned int IC = INSTRUCTION_COUNTER_INITIAL_ADDRESS; /* Instruction-Counter */
    unsigned int DC = 0; /* Data-Counter */
    unsigned int ERROR_FLAG = 0; /* Flag for error handling */

    /* Initialise tables and file pointers */
    symbol_table *symbols = NULL; /* Table for symbols */
    symbol_table *extern_symbols = NULL; /* Table for external symbols */
    macro_table * macros = NULL; /* Table for macros */
    FILE * as_src_file; /* Pointer to the source file to assemble */
    CPU assembler_cpu; /* CPU structure for assembly */

    printf("\nProccessing file %s:\n\n", filename);

    /* Run pre-assembler */
    printf("Running pre assembler on %s.as...\n", filename);
    if (pre_assembler(filename, &macros, &ERROR_FLAG))
        printf("Pre assembler stage complete, %s.am created.\n\n", filename);
    else
        printf("Quitting pre assembler stage due to error\n");

    /* Return if there were errors in pre-assembler */
    if (ERROR_FLAG)
        return 0;

    /* Open the .am created by pre assembler process file */
    if (!open_file(&as_src_file, filename, ".am", "r"))
        return 0;

    /* Perform first pass if no previous errors */
    if (!ERROR_FLAG) {
        printf("Preforming first pass on %s.am...\n", filename);
        if (first_pass(as_src_file, &assembler_cpu, &IC, &DC, &symbols, macros, &ERROR_FLAG))
            printf("First pass complete.\n\n");
        else
            printf("Quitting first pass stage due to error\n");
    }

    /* Perform second pass if no previous errors */
    if (!ERROR_FLAG) {
        printf("Preforming second pass on %s.am...\n", filename);
        if (second_pass(as_src_file, &assembler_cpu, &symbols, &extern_symbols, &ERROR_FLAG))
            printf("Second pass complete.\n\n");
        else
            printf("Quitting second pass stage due to error\n");
    }

    /* Create object file if no previous errors */
    if (!ERROR_FLAG) {
        if (create_object_file(filename, &assembler_cpu, &IC, &DC))
            printf("Object file %s.ob created.\n", filename);
        else
            printf("Error while creating objet file.\n");
    }

    /* Create entry file if no previous errors */
    if (!ERROR_FLAG) {
        if (create_file_for_entry_type(filename, symbols, entry_symbol, ".ent"))
            printf("Entry file %s.ent created.\n", filename);
    }

    /* Create external file if no previous errors */
    if (!ERROR_FLAG) {
        if (create_file_for_extern_type(filename, ".ext", extern_symbols))
            printf("Extern file %s.ext created.\n", filename);
    }

    printf("\n");
    free_macros(macros);
    free_symbols(symbols);
    free_symbols(extern_symbols);
    fclose(as_src_file);

    if (ERROR_FLAG)
        return 0;
    return 1; /* Return success */
}
