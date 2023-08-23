/* Created by Alex Persky. */

#ifndef ASSEMBLERPROJECT_SECOND_PASS_H
#define ASSEMBLERPROJECT_SECOND_PASS_H

#include "globals.h"
#include "table_management.h"

/**
 * Performs the second pass over of the assembler.
 * @param *src_file Pointer to source .am file that contains assembly instructions.
 * @param *assembler_cpu Pointer to the CPU structure where assembled code and data will be stored.
 * @param **symbols Double pointer to the symbol table.
 * @param **extern_symbols The table of external symbols, gets updated during second pass.
 * @param *ERROR_FLAG A flag to indicate if any error has occurred.
 * @return Success of the second pass, 0 if failed, 1 if succeeded.
 */
int second_pass(FILE* src_file, CPU * assembler_cpu, symbol_table ** symbols, symbol_table ** extern_symbols, unsigned int* ERROR_FLAG);

/**
 * Gets the size and addressing methods of an instruction from its machine code.
 * @param *assembler_cpu The CPU for the assembly.
 * @param IC The instruction counter.
 * @param *destination_addressing_method Pointer to store the destination addressing method.
 * @param *source_addressing_method Pointer to store the source addressing method.
 * @param *expected_operands Pointer to store the expected operands amount.
 * @return unsigned int The size of the instruction.
 */
unsigned int get_instruction_size_and_addressing_methods(CPU * assembler_cpu, unsigned int IC, unsigned int * destination_addressing_method, unsigned int * source_addressing_method, unsigned int * expected_operands);

/**
 * Sets the symbol code for a given address in the CPU instruction image.
 * @param *assembler_cpu The CPU for the assembly.
 * @param address The address to set the symbol code at.
 * @param *symbol The symbol to set.
 * @param **extern_symbols The table of external symbols.
 * @param **last_extern_symbol The last symbol in the table of external symbols.
 */
void set_symbol_code(CPU * assembler_cpu, unsigned int address, symbol_table * symbol, symbol_table ** extern_symbols, symbol_table ** last_extern_symbol);

/**
 * Processes entry commands, finds and sets given symbols as entry type.
 * @param **symbols The symbol table.
 * @param *current_line The current line being processed.
 * @param *offset The offset in the line.
 * @param line_counter The current line number.
 * @param *ERROR_FLAG A flag to signal if there's an error.
 */
void command_is_entry(symbol_table ** symbols, char * current_line, unsigned int * offset, unsigned int line_counter, unsigned int * ERROR_FLAG);

/**
 * Sets symbol code for instructions that use direct addressing.
 * @param *assembler_cpu The CPU for the assembly.
 * @param *IC The instruction counter.
 * @param *current_line The current line being processed.
 * @param **symbols The symbol table.
 * @param **extern_symbols The table of external symbols.
 * @param **last_extern_symbol The last symbol in the table of external symbols.
 * @param *offset The offset in the line.
 * @param line_counter The current line number.
 * @param *ERROR_FLAG A flag to signal if there's an error.
 */
void command_is_instruction(CPU * assembler_cpu, unsigned int * IC, char * current_line, symbol_table ** symbols, symbol_table ** extern_symbols, symbol_table ** last_extern_symbol, unsigned int * offset, unsigned int line_counter, unsigned int* ERROR_FLAG);

#endif /* ASSEMBLERPROJECT_SECOND_PASS_H */
