#ifndef ASSEMBLERPROJECT_FILE_UTILS_H
#define ASSEMBLERPROJECT_FILE_UTILS_H

#include "globals.h"
#include "table_management.h"

/**
 * Opens a file with the given parameters.
 * @param **file A pointer to a file pointer.
 * @param *filename The name of the file.
 * @param *filetype The type of the file.
 * @param *io_mode The file open mode.
 * @return 1 if successful, 0 if not.
 */
int open_file(FILE** file, char* filename, char* filetype, char* io_mode);

/**
 * Creates a .ob file with the given parameters.
 * @param *filename The name of the file.
 * @param *assembler_cpu The CPU struct.
 * @param *IC Instruction Counter.
 * @param *DC Data Counter.
 * @return 1 if successful, 0 if not.
 */
int create_object_file(char* filename, CPU * assembler_cpu, const unsigned int * IC, const unsigned int * DC);

/**
 * Creates a file for entry type symbols.
 * @param *filename The name of the file.
 * @param *symbols The symbol table.
 * @param type The type of symbols.
 * @param *filetype The type of the file.
 * @return 1 if successful, 0 if not.
 */
int create_file_for_entry_type(char* filename, symbol_table* symbols, symbol_type type, char* filetype);

/**
 * Creates a file for external type symbols.
 * @param *filename The name of the file.
 * @param *filetype The type of the file.
 * @param *extern_symbols The symbol table for external symbols.
 * @return 1 if successful, 0 if not.
 */
int create_file_for_extern_type(char* filename, char* filetype, symbol_table * extern_symbols);

/**
 * Checks if fgets buffer is exceeded.
 * @param *file The file to check.
 * @param *str The buffer string.
 * @return 1 if buffer exceeded, 0 if not.
 */
int is_fgets_buffer_exceeded(FILE * file, char * str);

/**
 * @brief Checks if fgets buffer is exceeded during copy, and copies source file data to destination file.
 * @param *src_file The source file.
 * @param *dst_file The destination file.
 * @param *str The buffer string.
 * @return 1 if buffer exceeded, 0 if not.
 */
int is_fgets_buffer_exceeded_copy(FILE * src_file, FILE * dst_file, char * str);

#endif /* ASSEMBLERPROJECT_FILE_UTILS_H */
