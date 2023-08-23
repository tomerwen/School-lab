/* Created by Alex Persky. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second_pass.h"
#include "globals.h"
#include "table_management.h"
#include "input_utils.h"
#include "error_handler.h"
#include "assembler_utils.h"

int second_pass(FILE * src_file, CPU * assembler_cpu, symbol_table ** symbols, symbol_table ** extern_symbols, unsigned int * ERROR_FLAG) {
    symbol_table * last_extern_symbol = NULL;
    char current_line[MAX_LINE_LENGTH + 2];
    char *symbol_name;
    char *command;
    unsigned int IC = INSTRUCTION_COUNTER_INITIAL_ADDRESS; /* Defining a counter to imitate IC, not the original IC counter. */
    unsigned int SYMBOL_DEF_FLAG;
    unsigned int offset;
    unsigned int line_counter = 0;

    /* Reset file to read from begining */
    fseek(src_file, 0, SEEK_SET);

    /* Read lines from file untill EOF */
    while (fgets(current_line, MAX_LINE_LENGTH + 2, src_file) != NULL) {
        line_counter++;
        offset = 0;
        SYMBOL_DEF_FLAG = 0;

        /* Ignore empty lines and commented lines */
        if (!is_empty_or_comment(current_line)) {
            /* Check if is label */
            if (read_word_or_up_to_char(current_line, &symbol_name, ':', &offset))
                 SYMBOL_DEF_FLAG = 1;

            command = read_word(current_line, &offset);

            /* First pass checks that all commands are legal therefor their validity isn't checked during second pass */
            if (strncmp(command, ".entry", 7) == 0) {
                if (SYMBOL_DEF_FLAG)
                    /* Symbols defined before '.entry' and 'extern' don't get stored as they are meaningless, meaning that they are free to be defined elsewhere in the code */
                    output_warning(symbol_definition_before_entry, line_counter);

                /* Process entry command */
                command_is_entry(symbols, current_line, &offset, line_counter, ERROR_FLAG);
            }
            /* Process instructions */
            else if (!((strncmp(command, ".data", 6) == 0) || (strncmp(command, ".string", 8) == 0) || (strncmp(command, ".extern", 8) == 0)))
                command_is_instruction(assembler_cpu, &IC, current_line, symbols, extern_symbols, &last_extern_symbol, &offset, line_counter, ERROR_FLAG);

            if (SYMBOL_DEF_FLAG)
                free(symbol_name);
            free(command);
        }
    }
    if (*ERROR_FLAG)
        return 0;
    return 1;
}

unsigned int get_instruction_size_and_addressing_methods(CPU * assembler_cpu, unsigned int IC, unsigned int * destination_addressing_method, unsigned int * source_addressing_method, unsigned int * expected_operands) {
    memory_cell cell = assembler_cpu->instruction_image[IC];
    unsigned int L = 3;

    /* To get first three bits after shift preform bitwase and 7 on shifter numbers */
    *destination_addressing_method = (cell.bits >> 2) & 7; /* Get bits 2-4 */
    *source_addressing_method = (cell.bits >> 9) & 7;  /* Get bits 9-11 */
    *expected_operands = 2; /* Initialize as 2 and reduce later */

    /* If noo source operand */
    if (*source_addressing_method == 0) {
        L--;
        *expected_operands = 1;
    }

    /* If no destination operand */
    if (*destination_addressing_method == 0) {
        L--;
        *expected_operands = 0;
    }

    /* If two register operands */
    if ((*destination_addressing_method == register_addressing) && (*source_addressing_method == register_addressing))
        L--;

    return L;
}

void set_symbol_code(CPU * assembler_cpu, unsigned int address, symbol_table * symbol, symbol_table ** extern_symbols, symbol_table ** last_extern_symbol) {
    symbol_table * symbol_copy;

    /* If extern type symbol */
    if (symbol->type == extern_symbol) {
        /* Set ARE */
        assembler_cpu->instruction_image[address].bits = external;

        /* Copy symbol into extern symbols table */
        symbol_copy = copy_symbol(symbol);
        symbol_copy->value = address;

        if (*extern_symbols == NULL) {
            /* If table is empty */
            *extern_symbols = symbol_copy;
            *last_extern_symbol = symbol_copy;
        }
        else {
            /* If table isnt empty, add symbol as last symbol */
            (*last_extern_symbol)->next = symbol_copy;
            *last_extern_symbol = (*last_extern_symbol)->next;
        }
    }
    else {
        /* If symbol isnt extern, encode the symbol address with ARE as relocatable */
        assembler_cpu->instruction_image[address].bits = symbol->value << 2;
        assembler_cpu->instruction_image[address].bits += relocatable;
    }
}

void command_is_entry(symbol_table ** symbols, char * current_line, unsigned int * offset, unsigned int line_counter, unsigned int * ERROR_FLAG) {
    symbol_table * symbol;
    char * parameter;
    int PARAMETER_ERROR_FLAG = 0;
    int SYMBOL_ERROR_FLAG = 0;
    int parameter_count = 0;
    int parameter_read_status;

    /* Read parameters until end line or util error occurs,  */
    parameter = read_word(current_line, offset);
    while (parameter != NULL && strlen(parameter) > 0 && !PARAMETER_ERROR_FLAG && !SYMBOL_ERROR_FLAG) {
        parameter_count++;

        /* Check if parameter is valid symbol name */
        if (!is_valid_symbol_macro_name(parameter)) {
            output_error(one_or_more_illegal_operands, line_counter, ERROR_FLAG);
            SYMBOL_ERROR_FLAG = 1;
        }
        else {
            /* Find symbol in symbols table */
            symbol = find_symbol_by_name(*symbols, parameter);
            if (symbol != NULL) {
                /* Symbol cant be defined both as extern and entry. */
                if (symbol->type != extern_symbol) {
                    symbol->type = entry_symbol;
                }
                else {
                    /* extern symbols are handled during first pass therefor all symbols defined both as entry and
                     * extern will output an error for the entry definition. */
                    output_error(symbol_already_defined_as_extern, line_counter, ERROR_FLAG);
                    SYMBOL_ERROR_FLAG = 1;
                }
            }
            else {
                /* Undefined entry symbols is an error */
                output_error(entry_symbol_undefined, line_counter, ERROR_FLAG);
                SYMBOL_ERROR_FLAG = 1;
            }
        }

        free(parameter);
        parameter = NULL;

        /* Read next parameter and check its validity */
        parameter_read_status = read_next_parameter(current_line, &parameter, offset);
        if (parameter_read_status == 1) {
            output_error(missing_comma, line_counter, ERROR_FLAG);
            PARAMETER_ERROR_FLAG = 1;
        }
        if (parameter_read_status == 2) {
            output_error(one_or_more_operands_missing, line_counter, ERROR_FLAG);
            PARAMETER_ERROR_FLAG = 1;
        }
    }

    if (parameter != NULL)
        free(parameter);

    /* .entry command requires at lest one operand */
    if (parameter_count == 0)
        output_error(one_or_more_operands_missing, line_counter, ERROR_FLAG);
}

void command_is_instruction(CPU * assembler_cpu, unsigned int * IC, char * current_line, symbol_table ** symbols, symbol_table ** extern_symbols, symbol_table ** last_extern_symbol, unsigned int * offset, unsigned int line_counter, unsigned int* ERROR_FLAG)
{
    symbol_table * symbol = NULL;
    char *first_operand = NULL;
    char *second_operand = NULL;
    unsigned int L;
    unsigned int operand_count;
    unsigned int destination_addressing_method;
    unsigned int source_addressing_method;
    unsigned int expected_operands;

    /* Get the size of the instruction in the instruction image, and its addresing methods */
    L = get_instruction_size_and_addressing_methods(assembler_cpu, *IC, &destination_addressing_method, &source_addressing_method, &expected_operands);

    /* Read operands */
    read_operands(current_line, &first_operand, &second_operand, offset, &operand_count, expected_operands, line_counter, ERROR_FLAG);

    /* If the addressing method is direct for an operand, find the label and set its code in the instruction image */
    if (expected_operands == 1) {
        /* If instruction has one operand, check only destination addressing method */
        if (destination_addressing_method == direct_addressing) {
            symbol = find_symbol_by_name(*symbols, first_operand);
            if (symbol != NULL) {
                /* A situation where the symbol given as an operand isn't a fitting type for the instruction is
                 * undefined therefor isn't necessarily an error  */
                set_symbol_code(assembler_cpu, (*IC) + 1, symbol, extern_symbols, last_extern_symbol);
            }
            else
                output_error(symbol_undefined, line_counter, ERROR_FLAG);
        }
    }
    else if (expected_operands == 2) {
        /* If instruction has two operands, check both source and destination addressing methods */
        if (source_addressing_method == direct_addressing) {
            symbol = find_symbol_by_name(*symbols, first_operand);
            if (symbol != NULL)
                /* A situation where the symbol given as an operand isn't a fitting type for the instruction is
                 * undefined therefor isn't necessarily an error  */
                set_symbol_code(assembler_cpu, (*IC) + 1, symbol, extern_symbols, last_extern_symbol);
            else
                output_error(symbol_undefined, line_counter, ERROR_FLAG);
        }
        if (destination_addressing_method == direct_addressing)
        {
            symbol = find_symbol_by_name(*symbols, second_operand);
            if (symbol != NULL)
                /* A situation where the symbol given as an operand isn't a fitting type for the instruction is
                 * undefined therefor isn't necessarily an error  */
                set_symbol_code(assembler_cpu, (*IC) + 2, symbol, extern_symbols, last_extern_symbol);
            else
                output_error(symbol_undefined, line_counter, ERROR_FLAG);
        }
    }

    /* Increment the local instruction counter */
    (*IC) += L;

    if (first_operand != NULL)
        free(first_operand);
    if (second_operand != NULL)
        free(second_operand);
}