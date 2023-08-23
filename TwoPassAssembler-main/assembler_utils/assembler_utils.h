#ifndef ASSEMBLERPROJECT_ASSEMBLER_UTILS_H
#define ASSEMBLERPROJECT_ASSEMBLER_UTILS_H

#include "globals.h"

/**
 * Returns the opcode value of the string representing an instruction.
 * @param *command A pointer to a character array representing the assembly instruction.
 * @return Returns the numerical representation of the assembly instruction, or -1 if it's invalid.
 */
int find_instruction(const char * command);

/**
 * Checks if a macro or symbol name are valid
 * @param *name A pointer to a character array representing the name of the macro or symbol.
 * @return Returns 1 if the macro or symbol name is valid, 0 otherwise.
 */
int is_valid_symbol_macro_name(char* name);

/**
 * Encodes instruction into a 12 bit memory cell.
 * @param ARE An unsigned integer representing the ARE of the instruction.
 * @param destination_operand An unsigned integer representing the destination operand.
 * @param opcode An unsigned integer representing the opcode.
 * @param source_operand An unsigned integer representing the source operand.
 * @return Returns a pointer to a memory cell containing the encoded instruction.
 */
memory_cell * encode_instructions(unsigned int ARE, unsigned int destination_operand, unsigned int opcode, unsigned int source_operand);

/**
 * Encodes data parameter into a 12 bit memory cell.
 * @param parameter An integer representing the data parameter to be encoded.
 * @return Returns a pointer to a memory cell containing the encoded data parameter.
 */
memory_cell * encode_data_parameter(int parameter);

/**
 * Encodes 12 bits of an int into its 64 bit representation.
 * @param num An unsigned integer representing a 12-bit number.
 * @return Returns a pointer to a character array containing the base64-encoded representation of the number.
 */
char* base64_encode_12bit(unsigned int num);

#endif /* ASSEMBLERPROJECT_ASSEMBLER_UTILS_H */
