/* Created by Alex Persky. */

#ifndef ASSEMBLERPROJECT_ERROR_HANDLER_H
#define ASSEMBLERPROJECT_ERROR_HANDLER_H

/**
 * Outputs an error message and sets the ERROR_FLAG.
 * @param error An error code.
 * @param line_number The line number where the error occurred.
 * @param ERROR_FLAG A pointer to the flag indicating the occurrence of an error.
 */
void output_error(unsigned int error, unsigned int line_number, unsigned int * ERROR_FLAG);

/**
 * Outputs a warning message.
 * @param warning A warning code.
 * @param line_number The line number where the warning occurred.
 */
void output_warning(unsigned int warning, unsigned int line_number);

#endif /* ASSEMBLERPROJECT_ERROR_HANDLER_H */
