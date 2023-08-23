#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input_utils.h"
#include "../error_handler/error_handler.h"
#include "globals.h"

char * skip_spaces(char * str, unsigned int * offset) {
    /* Read string while its a space and isn't end of line or file */
    while (*str != '\0' && *str != '\n' && isspace((char)*str)) {
        str++;
        (*offset)++;
    }
    /* Returns address of first non-whitespace character */
    return str;
}

char * str_concat_ret(const char * src1, const char * src2) {
    char *str;

    /* Allocate memory for string to that will contain the concatenation result */
    str = malloc((strlen(src1) + strlen(src2) + 1) * sizeof(char)); /* Space for length plus null */
    if (str == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    /* Copy the characters */
    strcpy(str, src1);
    strcat(str, src2);

    /* Return the new string */
    return str;
}

char * read_word(char * str, unsigned int * offset) {
    char* end;
    char* word;
    int len;

    /* Skip initial spaces and tabs. */
    str = skip_spaces(str+(*offset), offset);

    /* Find the address of the end of the word */
    end = str;
    while (*end != '\0' && *end != ' ' && *end != ',' && *end != '\t' && *end != '\n') {
        end++;
    }

    /* Allocate memory for the new string. */
    len = (int)(end - str);
    word = (char*)malloc((len + 1) * sizeof(char));
    if (word == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Copy the characters to the new string and null-terminate it. */
    strncpy(word, str, len);
    word[len] = '\0';

    /* Update offset and return the allocated word */
    *offset += len;
    return word;
}

int read_word_or_up_to_char(char * str, char ** word, char c, unsigned int * offset) {
    /* Find the first space or newline character in the string. */
    char* end;
    int len;
    unsigned int offset_temp = *offset;

    /* Skip initial spaces and tabs. */
    str = skip_spaces(str, offset);

    /* Find the address of the end of the word, or of the character */
    end = str;
    while (*end != c && *end != '\0' && *end != ' ' && *end != ',' && *end != '\t' && *end != '\n') {
        end++;
    }

    /* If character wasn't in the first word returns 0 and doesn't save the word */
    if (*end != c) {
        *word = NULL;
        *offset = offset_temp;
        return 0;
    }

    /* offset the character */
    (*offset)++;

    /* Allocate memory for the new string. */
    len = (int)(end - str);
    *word = (char*)malloc((len + 1) * sizeof(char));
    if (word == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Copy the characters to the new string and null-terminate it */
    strncpy(*word, str, len);
    (*word)[len] = '\0';

    /* Update offset and return the allocated word */
    *offset += len;
    return 1;
}

int read_comma(char * str, unsigned int * offset) {
    /* Skip initial spaces and tabs. */
    str = skip_spaces(str+(*offset), offset);

    /* If comma wasn't found returns 0 */
    if (str[0] != ',')
        return 0;

    /* If comma found increments offset and returns 1 */
    (*offset)++;
    return 1;
}

int is_empty(char * str) {
    unsigned int offset = 0;

    /* Skip initial spaces and tabs. */
    str = skip_spaces(str+offset, &offset);

    /* If line is empty after passing whitespaces returns 1, otherwise 0 */
    if ((strlen(str) == 0) || (str[0] == '\n'))
        return 1;
    return 0;
}

int is_empty_or_comment(char * str) {
    unsigned int offset = 0;

    /* Skip initial spaces and tabs. */
    str = skip_spaces(str+offset, &offset);

    /* If line is empty after passing whitespaces, or starts with ';' returns 1, otherwise 0 */
    if (is_empty(str) || (str[0] == ';'))
        return 1;
    return 0;
}

int is_ascii(int c) {
    /* Returns if the int value of the character is between 0 and 127 */
    return (c >= 0 && c <= 127);
}

int read_next_parameter(char * str, char ** parameter, unsigned int * offset) {
    if (!is_empty(str + *offset))
    {
        if (!read_comma(str, offset))
            /* If comma is missing returns 1 */
            return 1;
        else {
            *parameter = read_word(str, offset);
            if (strlen(*parameter) == 0)
                /* If parameter is missing after comma returns 2 */
                return 2;
            else
                /* If parameter and comma were read successfully returns 3 */
                return 3;
        }
    }
    else
        /* If line is empty returns 0 */
        return 0;
}

char * read_string_parameter(char * str, unsigned int * offset, int * ERROR_FLAG) {
    char* word;
    char* end;
    char* ptr;
    unsigned int len;

    /* Skip initial spaces and tabs. */
    str = skip_spaces(str+(*offset), offset);

    /* String parameter has to start with quotation mark */
    if (str[0] != '\"')
        *ERROR_FLAG = 1;

    /* Offset quotation mark */
    str++;
    (*offset)++;

    /* Search quotation mark from the end of the string (string can contain more quotation marks between the outermost two) */
    end = str + strlen(str) - 1;
    while (*end != '\"' && end != str)
        end--;

    /* No closing quotation mark found */
    if (end == str)
        *ERROR_FLAG = 1;

    /* Check if all characters are ascii */
    for (ptr = str; ptr != end; ptr++)
        if (!is_ascii(*ptr))
            *ERROR_FLAG = 1;

    /* Returns NULL if error */
    if (*ERROR_FLAG)
        return NULL;

    /* Allocate memory for new word */
    len = (int)(end - str);
    word = (char*)malloc((len + 1) * sizeof(char));
    if (word == NULL) {
        /* Handle malloc failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Copy the characters to the new string and null-terminate it. */
    strncpy(word, str, len);
    word[len] = '\0';

    /* Update offset and return the allocated word */
    *offset += len;
    return word;
}

int read_operands(char * current_line, char ** first_operand, char ** second_operand, unsigned int * offset, unsigned int * operand_count, unsigned int expected_operands, unsigned int line_counter, unsigned int * ERROR_FLAG) {
    unsigned int COMMA_ERROR_FLAG = 0;

    *operand_count = 0;
    *first_operand = read_word(current_line, offset);

    /* Check if first operand was read */
    if (strlen(*first_operand) > 0) {
        (*operand_count)++;
        /* If more operands are expected, read a comma and the second operand */
        if ((*operand_count) < expected_operands) {
            if (read_comma(current_line, offset)) {
                *second_operand = read_word(current_line, offset);
                /* Check if second operand was read */
                if (strlen(*second_operand) > 0)
                    (*operand_count)++;
            }
            else {
                /* If second operand exists and theres no comma between them, set COMMA_ERROR_FLAG to 1 */
                *second_operand = read_word(current_line, offset);
                if (strlen(*second_operand) > 0)
                    (*operand_count)++;

                COMMA_ERROR_FLAG = 1;
            }
        }
    }

    if ((*operand_count) < expected_operands)
        output_error(one_or_more_operands_missing, line_counter, ERROR_FLAG);
    else if (COMMA_ERROR_FLAG) {
        output_error(missing_comma, line_counter, ERROR_FLAG);
    }
    else if (!is_empty(current_line + (*offset)))
        output_error(instruction_excess_operands, line_counter, ERROR_FLAG);

    /* Returns 1 if enough operands were read, otherwise false */
    return *operand_count == expected_operands;
}

int is_valid_number(const char * parameter) {
    int i;
    int value;

    /* If string is empty its not a number */
    if (strlen(parameter) == 0)
        return 0;

    /* If string is doesnt start with '-' or a digit its not a number */
    if (parameter[0] != '-' && !isdigit((unsigned char)parameter[0]))
        return 0;

    /* If string contains only '-' its not a number */
    if (parameter[0] == '-' && strlen(parameter) == 1)
        return 0;

    /* Check if all characters except the first are digits */
    for (i = 1; parameter[i] != '\0'; i++) {
        if (!isdigit((unsigned char)parameter[i])) {
            return 0;
        }
    }

    /* Check if number is within max and min values */
    value = atoi(parameter);
    if (value > MAX_INT_SIZE || value < MIN_INT_SIZE)
        return 0;

    /* Returns 1 if string is a number, otherwise returns 0 */
    return 1;
}