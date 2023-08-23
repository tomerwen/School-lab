/* Created by Alex Persky. */

#ifndef ASSEMBLERPROJECT_PRE_ASSEMBLER_H
#define ASSEMBLERPROJECT_PRE_ASSEMBLER_H

#include "table_management.h"

/**
 * Preforms initial step of the assembler, scans and replaces macros in the source file.
 * @param *filename The name of the assembly file to be processed.
 * @param **macros A double pointer to the macro_table structure where the discovered macros will be stored.
 * @param *ERROR_FLAG A pointer to an integer flag which is set if an error occurs.
 * @return 1 if pre-processing succeeds without error, 0 otherwise.
 */
int pre_assembler(char* filename, macro_table ** macros, unsigned int * ERROR_FLAG);

/**
 * Checks if the given string starts with the word 'mcro'.
 * @param *str The string to be checked.
 * @return 1 if the string starts with 'mcro', 0 otherwise.
 */
int starts_with_mcro(char* str);

/**
 * Checks if the given string starts with the word 'endmcro'.
 * @param *str The string to be checked.
 * @return 1 if the string starts with 'endmcro', 0 otherwise.
 */
int starts_with_endmcro(char* str);

#endif /* ASSEMBLERPROJECT_PRE_ASSEMBLER_H */
