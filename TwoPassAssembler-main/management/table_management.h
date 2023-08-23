#ifndef ASSEMBLERPROJECT_TABLE_MANAGEMENT_H
#define ASSEMBLERPROJECT_TABLE_MANAGEMENT_H

#include "globals.h"

/**
 * Structure defines a macro table, implemented using a linked list.
 * Each macro table node contains a name, text and a pointer to the next node.
 */
typedef struct macro_table {
    char * name; /* Macro name. */
    char * text; /* Macro text. */
    struct macro_table* next; /* Pointer to the next entry in the table. */
} macro_table;

/**
 * Structure defines a symbol table, implemented using a linked list.
 * Each symbol table node contains a name, type, value, and a pointer to the next node.
 */
typedef struct symbol_table {
    char name[MAX_SYMBOL_LENGTH]; /* symbol name. */
    symbol_type type; /* Type of the symbol. */
    unsigned int value; /* Symbol address. */
    struct symbol_table* next; /* Pointer to the next entry in the table. */
} symbol_table;

/**
 * Adds a new macro to the macro table.
 * @param *head A double pointer to the head of the macro table.
 * @param *name The name of the new macro to be added.
 * @param *text The text of the new macro to be added.
 */
void add_macro(macro_table** head, char* name, char* text);

/**
 * Finds a macro in the macro table using its name.
 * @param *head A pointer to the head of the macro table.
 * @param *name The name of the macro to be found.
 * @return text string of the found macro. NULL if the macro is not found.
 */
char* find_macro(macro_table* head, const char* name);

/**
 * Frees the memory allocated for the macro table.
 * @param *head A pointer to the head of the macro table.
 */
void free_macros(macro_table* head);

/**
 * @brief This function adds a new symbol to the symbol table.
 * @param **head A double pointer to the head of the symbol table.
 * @param *name The name of the new symbol to be added.
 * @param type The type of the new symbol to be added.
 * @param value The value of the new symbol to be added.
 */
void add_symbol(symbol_table** head, const char* name, symbol_type type, unsigned int value);

/**
 * Finds a symbol in the symbol table using its name.
 * @param *head A pointer to the head of the symbol table.
 * @param *name The name of the symbol to be found.
 * @return the found symbol. NULL if the symbol is not found.
 */
symbol_table * find_symbol_by_name(symbol_table* head, const char* name);

/**
 * Finds a non-extern type symbols in the symbol table using its name.
 * @param *head A pointer to the head of the symbol table.
 * @param *name The name of the non-extern symbol to be found.
 * @return the found non-extern type symbol. NULL if the symbol is not found.
 */
symbol_table * find_non_extern_symbol_by_name(symbol_table* head, const char* name);

/**
 * Creates a copy of a symbol.
 * @param *symbol A pointer to the symbol to be copied.
 * @return pointer to the new copy of the symbol.
 */
symbol_table* copy_symbol(symbol_table* symbol);

/**
 * Finds all symbols of a specific type in the symbol table.
 * @param *head A pointer to the head of the symbol table.
 * @param type The type of the symbols to be found.
 * @param *list_size A pointer to an integer where the size of the new list will be stored.
 * @return pointer to the head of the new list.
 */
symbol_table* find_symbols_by_type(symbol_table* head, symbol_type type, int* list_size);

/**
 * Frees the memory allocated for the symbol table.
 * @param *head A pointer to the head of the symbol table.
 */
void free_symbols(symbol_table* head);

/**
 * This function adds the instruction counter (IC) value to all data symbols in the symbol table.
 * @param symbol_table* head A pointer to the head of the symbol table.
 * @param unsigned int IC The instruction counter value to be added.
 * @return void
 */
void add_ic_to_data_symbols(symbol_table* head, unsigned int IC);

#endif /* ASSEMBLERPROJECT_TABLE_MANAGEMENT_H */
