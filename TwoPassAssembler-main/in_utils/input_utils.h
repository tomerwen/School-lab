#ifndef ASSEMBLERPROJECT_INPUT_UTILS_H
#define ASSEMBLERPROJECT_INPUT_UTILS_H

/**
 * Skips whitespaces in beggining of string.
 * @param *str The string from which to skip spaces.
 * @param *offset The offset at which to start, gets updated during use.
 * @return A pointer to the string after skipping the spaces.
 */
char* skip_spaces(char* str, unsigned int * offset);

/**
 * Concatenates two strings and returns a new string.
 * @param *src1 The first source string.
 * @param *src2 The second source string.
 * @return A newly allocated string containing the concatenation of src1 and src2.
 */
char * str_concat_ret(const char *src1, const char *src2);

/**
 * Reads a word from a string, skipping leading spaces.
 * @param *str The string from which to read the word.
 * @param *offset The offset at which to start.
 * @return A newly allocated string containing the word.
 */
char * read_word(char * str, unsigned int * offset);

/**
 * Reads a word from a string up to a specific character, or up to whitespaces.
 * @param *str The string from which to read the word.
 * @param **word A pointer to a char pointer to store the word.
 * @param c The character at which to stop reading.
 * @param *offset The offset at which to start.
 * @return 1 if successful, 0 otherwise.
 */
int read_word_or_up_to_char(char * str, char ** word, char c, unsigned int * offset);

/**
 * Checks if the next character in the string is a comma, while ignoring whitespaces.
 * @param *str The string to check.
 * @param *offset The offset at which to start.
 * @return 1 if the next character is a comma, 0 otherwise.
 */
int read_comma(char * str, unsigned int * offset);

/**
 * Checks if a string is empty (contains only whitespaces).
 * @param *str The string to check.
 * @return 1 if the string is empty, 0 otherwise.
 */
int is_empty(char * str);

/**
 * Checks if a string is empty or a comment.
 * @param *str The string to check.
 * @return 1 if the string is empty or a comment, 0 otherwise.
 */
int is_empty_or_comment(char * str);

/**
 * Checks if a caracter is ascii.
 * @param c The char to check.
 * @return 1 if the char is ascii, 0 otherwise.
 */
int is_ascii(int c);

/**
 * Reads the next parameter from a string.
 * @param *str The string from which to read the parameter.
 * @param **parameter A pointer to a char pointer to store the parameter.
 * @param *offset The offset at which to start.
 * @return 0 if the line is empty, 1 if the comma is missing, 2 if the parameter is missing after comma, 3 if parameter and comma were read successfully.
 */
int read_next_parameter(char * str, char ** parameter, unsigned int * offset);

/**
 * Reads a string parameter from between the outermost quotation marks.
 * @param *str The string from which to read the parameter.
 * @param *offset The offset at which to start.
 * @param *ERROR_FLAG A pointer to an int to store any error.
 * @return A newly allocated string containing the parameter.
 */
char * read_string_parameter(char * str, unsigned int * offset, int * ERROR_FLAG);

/**
 * Reads zero to two operands from a line.
 * @param *current_line The line from which to read the operands.
 * @param **first_operand A pointer to a char pointer to store the first operand.
 * @param **second_operand A pointer to a char pointer to store the second operand.
 * @param *offset The offset at which to start.
 * @param *operand_count A pointer to an unsigned int to store the operand count.
 * @param expected_operands The number of expected operands.
 * @param line_counter The current line number.
 * @param *ERROR_FLAG A pointer to an unsigned int to store any error.
 * @return 1 if the operand count matches the expected operands, 0 otherwise.
 */
int read_operands(char * current_line, char ** first_operand, char ** second_operand, unsigned int * offset, unsigned int * operand_count, unsigned int expected_operands, unsigned int line_counter, unsigned int * ERROR_FLAG);

/**
 * Checks if a string represents a valid number.
 * @param parameter The string to check.
 * @return 1 if the string represents a valid number, 0 otherwise.
 */
int is_valid_number(const char * parameter);

#endif /* ASSEMBLERPROJECT_INPUT_UTILS_H */
