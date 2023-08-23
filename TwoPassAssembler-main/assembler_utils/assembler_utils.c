#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assembler_utils.h"
#include "globals.h"

int find_instruction(const char * command) {
    /* Check if the command length is 3, if yes then look for a match in the list of 3-letter instructions and return its code */
    if (strlen(command) == 3) {
        if (strncmp(command, "mov", 3) == 0)
            return mov;
        else if (strncmp(command, "cmp", 3) == 0)
            return cmp;
        else if (strncmp(command, "add", 3) == 0)
            return add;
        else if (strncmp(command, "sub", 3) == 0)
            return sub;
        else if (strncmp(command, "lea", 3) == 0)
            return lea;
        else if (strncmp(command, "not", 3) == 0)
            return not;
        else if (strncmp(command, "clr", 3) == 0)
            return clr;
        else if (strncmp(command, "inc", 3) == 0)
            return inc;
        else if (strncmp(command, "dec", 3) == 0)
            return dec;
        else if (strncmp(command, "jmp", 3) == 0)
            return jmp;
        else if (strncmp(command, "bne", 3) == 0)
            return bne;
        else if (strncmp(command, "red", 3) == 0)
            return red;
        else if (strncmp(command, "prn", 3) == 0)
            return prn;
        else if (strncmp(command, "jsr", 3) == 0)
            return jsr;
        else if (strncmp(command, "rts", 3) == 0)
            return rts;
    }
    /* Check if the command is "stop". If yes, return its code */
    if (strlen(command) == 4 && strncmp(command, "stop", 4) == 0)
        return stop;

    /* If the command does not match any known command, return -1 */
    else
        return -1;
}

int is_valid_symbol_macro_name(char* name) {
    int i;

    /* If the name length exceeds the maximum symbol length, return false */
    if (strlen(name) > MAX_SYMBOL_LENGTH)
        return 0;

    /* Check if the first character is not a letter, if true then return false */
    if (name[0] != '\0' && !isalpha((unsigned char)name[0]))
        return 0;

    /* Check each subsequent character, if it's not alphanumeric, return false */
    for (i = 1; name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)name[i])) {
            return 0;
        }
    }

    /* If the name matches any saved words, return false */
    if (find_instruction(name) != -1)
        return 0;

    if ((strncmp(name, ".data", 7) == 0) ||
        (strncmp(name, ".string", 7) == 0) ||
        (strncmp(name, ".entry", 7) == 0) ||
        (strncmp(name, ".extern", 7) == 0))
        return 0;

    /* Return true if valid name */
    return 1;
}

memory_cell * encode_instructions(unsigned int ARE, unsigned int destination_operand, unsigned int opcode, unsigned int source_operand) {
    memory_cell * cell = malloc(sizeof(memory_cell));
    if (cell == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Encode ARE, destination operand, opcode, and source operand according to their bits in memory */
    cell->bits = 0;
    cell->bits += ARE; /* bits 0-1 */
    cell->bits += destination_operand << 2; /* bits 2-4 */
    cell->bits += opcode << 5; /* bits 5-8 */
    cell->bits += source_operand << 9; /* bits 9-11 */

    return cell;
}

memory_cell * encode_data_parameter(int parameter) {
    memory_cell * cell = malloc(sizeof(memory_cell));
    if (cell == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Each data parameter takes up all 12 bits of a memory cell */
    cell->bits = parameter;
    return cell;
}

char* base64_encode_12bit(unsigned int num) {
    /* Define the base64 characters mask */
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    char *result = malloc(3);
    if (result == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Encode the first 6 bits of the number and add the corresponding base64 character to the result string */
    result[0] = base64_chars[(num >> 6) & 0x3F];

    /* Encode the last 6 bits of the number and add the corresponding base64 character to the result string */
    result[1] = base64_chars[num & 0x3F];

    /* Null-terminate the string */
    result[2] = '\0';

    return result;
}
