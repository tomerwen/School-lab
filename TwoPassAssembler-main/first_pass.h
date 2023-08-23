/* Created by Alex Persky. */

#ifndef ASSEMBLERPROJECT_FIRST_PASS_H
#define ASSEMBLERPROJECT_FIRST_PASS_H

#include "globals.h"
#include "table_management.h"

/**
 * Preforms the first pass of the assembler.
 * @param *src_file Pointer to source .am file that contains assembly instructions.
 * @param *assembler_cpu Pointer to the CPU structure where assembled code and data will be stored.
 * @param *IC Instruction Counter, counts the amount of memory cells the instructions take up.
 * @param *DC Data Counter, counts the amount of memory cells the data takes up.
 * @param **symbols Double pointer to the symbol table.
 * @param *macros Pointer to the macros table.
 * @param *ERROR_FLAG A flag to indicate if any error has occurred.
 * @return Success of the first pass, 0 if failed, 1 if succeeded.
 */
int first_pass(FILE* src_file, CPU* assembler_cpu, unsigned int* IC, unsigned int* DC, symbol_table** symbols, macro_table* macros, unsigned int* ERROR_FLAG);

/**
 * Checks if the given string is a numeric parameter, and saves it.
 * @param *str Pointer to the string to check.
 * @param *value If str is a number, the function will set *value to the numeric value of str.
 * @return 1 if the string is a numeric parameter, 0 if not.
 */
int is_parameter_number(const char* str, int* value);

/**
 * Checks if the given string is a register parameter.
 * @param *str Pointer to the string to check.
 * @param *operand_as_register If str is a register, the function will set *operand_as_register to the register's numeric value.
 * @param *ERROR_FLAG A flag to indicate if any error has occurred.
 * @return 1 if the string is a register parameter, 0 if not.
 */
int is_parameter_register(const char* str, int* operand_as_register, unsigned int* ERROR_FLAG);

/**
 * Gets addressing method of the given operand.
 * @param *operand Pointer to the operand string.
 * @param *operand_as_number Pointer to the integer that will be set if the operand is a number.
 * @param *operand_as_register Pointer to the integer that will be set if the operand is a register.
 * @return The addressing method as an integer.
 */
int get_addressing_method(char* operand, int* operand_as_number, int* operand_as_register);

/**
 * Processes operands of instructions and converts them to their machine code.
 * @param *operand Pointer to the operand string.
 * @param *addressing_method Pointer to the integer that represents the addressing method.
 * @param dest_src Integer that indicates whether the operand is source or destination.
 * @param *ERROR_FLAG Pointer to the error flag.
 * @return The memory cell containing the machine code of the operand.
 */
memory_cell * process_operand(char* operand, int* addressing_method, int dest_src, unsigned int* ERROR_FLAG);

/**
 * Merges register codes into one.
 * @param *first_code Pointer to the first memory cell.
 * @param *second_code Pointer to the second memory cell.
 * @return The merged memory cell.
 */
memory_cell * merge_register_codes(memory_cell * first_code, memory_cell * second_code);

/**
 * Checks if the command is an instruction, if it is then processes it and saves it to instruction image.
 * @param *assembler_cpu Pointer to the CPU structure.
 * @param *IC Pointer to the Instruction Counter.
 * @param *DC Pointer to the Data Counter.
 * @param *current_line Pointer to the current line string.
 * @param *command Pointer to the command string.
 * @param *offset Pointer to the offset integer.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 */
void process_instruction(CPU * assembler_cpu, unsigned int * IC, unsigned int * DC, char * current_line, char * command, unsigned int * offset,unsigned int line_counter, unsigned int* ERROR_FLAG);

/**
 * Validates the addressing method of the operands for the instruction
 * @param instruction Pointer to the CPU structure.
 * @param source_addressing_method Pointer to the Instruction Counter.
 * @param destination_addressing_method Pointer to the Data Counter.
 */
int validate_addressing_method_per_instruction(int instruction, int source_addressing_method, int destination_addressing_method);

/**
 * Saves the instruction to instruction image.
 * @param *assembler_cpu Pointer to the CPU structure.
 * @param *IC Pointer to the Instruction Counter.
 * @param *DC Pointer to the Data Counter.
 * @param *first_operand Pointer to the first operand string.
 * @param *second_operand Pointer to the second operand string.
 * @param instruction Integer that represents the instruction.
 * @param operands_count Integer that represents the number of operands.
 * @param *L Pointer to the L integer.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 */
void save_instruction_to_memory(CPU * assembler_cpu, unsigned int * IC, const unsigned int * DC, char * first_operand, char * second_operand, int instruction, int operands_count, unsigned int * L, unsigned int line_counter, unsigned int* ERROR_FLAG);

/**
 * Reads and processes data parameters of data command.
 * @param *assembler_cpu Pointer to the CPU structure.
 * @param *IC Pointer to the Instruction Counter.
 * @param *DC Pointer to the Data Counter.
 * @param *current_line Pointer to the current line string.
 * @param *offset Pointer to the offset integer.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 */
void proccess_data(CPU * assembler_cpu, const unsigned int * IC, unsigned int * DC, char * current_line, unsigned int * offset, unsigned int line_counter, unsigned int * ERROR_FLAG);

/**
 * Reads and processes string parameters of string command.
 * @param *assembler_cpu Pointer to the CPU structure.
 * @param *IC Pointer to the Instruction Counter.
 * @param *DC Pointer to the Data Counter.
 * @param *current_line Pointer to the current line string.
 * @param *offset Pointer to the offset integer.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 */
int proccess_string(CPU * assembler_cpu, const unsigned int * IC, unsigned int * DC, char * current_line, unsigned int * offset, unsigned int line_counter, unsigned int * ERROR_FLAG);

/**
 * Checks if the command is entry or extern, if extern reads and processes all parameters.
 * @param *command Pointer to the command string.
 * @param *current_line Pointer to the current line string.
 * @param *offset Pointer to the offset integer.
 * @param **symbols Double pointer to the symbol table.
 * @param SYMBOL_DEF_FLAG Unsigned integer that represents the symbol definition flag.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 * @return Whether the command is entry or extern.
 */
int command_is_entry_or_extern(char * command, char* current_line, unsigned int * offset, symbol_table **symbols, unsigned int SYMBOL_DEF_FLAG, unsigned int line_counter, unsigned int * ERROR_FLAG);

/**
 * Checks if the command is data or string, calls functions accordingly.
 * @param *assembler_cpu Pointer to the CPU structure.
 * @param *command Pointer to the command string.
 * @param *symbol_name Pointer to the symbol name string.
 * @param *current_line Pointer to the current line string.
 * @param *offset Pointer to the offset integer.
 * @param **symbols Double pointer to the symbol table.
 * @param *IC Pointer to the Instruction Counter.
 * @param *DC Pointer to the Data Counter.
 * @param line_counter Unsigned integer that counts the lines.
 * @param *ERROR_FLAG Pointer to the error flag.
 * @param SYMBOL_DEF_FLAG Unsigned integer that represents the symbol definition flag.
 * @return Whether the command is data or string.
 */
int command_is_data_or_string(CPU * assembler_cpu, char * command, char * symbol_name, char* current_line, unsigned int * offset, symbol_table **symbols, unsigned int * IC, unsigned int * DC, unsigned int line_counter, unsigned int * ERROR_FLAG, unsigned int SYMBOL_DEF_FLAG);

#endif /* ASSEMBLERPROJECT_FIRST_PASS_H */
