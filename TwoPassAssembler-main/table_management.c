/* Created by Alex Persky. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table_management.h"
#include "globals.h"

void add_macro(macro_table** head, char* name, char* text) {
    /* Allocate memory for macro table node */
    macro_table* node = (macro_table*) malloc(sizeof(macro_table));
    if (node == NULL) {
        /* Handle allocation failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Set node values */
    node->name = name; /* Set pointer to name */
    node->text = text; /* Set pointer to text */
    node->next = *head;
    *head = node;
}

char* find_macro(macro_table* head, const char* name) {
    /* Search macro_table list for node with same name as parameter */
    while (head != NULL) {
        if (strcmp(head->name, name) == 0)
            return head->text;
        head = head->next;
    }
    return NULL; /* Return NULL if the symbol is not found. */
}

void free_macros(macro_table* head) {
    macro_table* tmp;

    /* Free nodes in list */
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->text);
        free(tmp);
    }
}

void add_symbol(symbol_table** head, const char* name, symbol_type type, unsigned int value) {
    /* Allocate memory for symbol table node */
    symbol_table* node = (symbol_table*) malloc(sizeof(symbol_table));
    if (node == NULL) {
        /* Handle allocation failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Set node values, and the node as the new head */
    strncpy(node->name, name, MAX_SYMBOL_LENGTH - 1); /* Copy name. */
    node->name[MAX_SYMBOL_LENGTH - 1] = '\0'; /* Ensure null-termination. */
    node->type = type;
    node->value = value;
    node->next = *head;
    *head = node;
}

symbol_table * find_symbol_by_name(symbol_table* head, const char* name) {
    /* Search symbol_table list for node with same name as parameter */
    while (head != NULL && name != NULL) {
        if (strcmp(head->name, name) == 0)
            return head; /* Return node with fitting name */
        head = head->next;
    }
    return NULL; /* Return NULL if the symbol is not found. */
}

symbol_table * find_non_extern_symbol_by_name(symbol_table* head, const char* name) {
    /* Search symbol_table list for node with same name as parameter and of type that isnt extern */
    while (head != NULL && name != NULL) {
        if (head->type != extern_symbol && strcmp(head->name, name) == 0)
            return head; /* Return node with fitting name and type */
        head = head->next;
    }
    return NULL; /* Return NULL if the symbol is not found. */
}

symbol_table* copy_symbol(symbol_table* symbol) {
    /* Allocate memory for symbol table node */
    symbol_table* new_symbol = (symbol_table*) malloc(sizeof(symbol_table));
    if (new_symbol == NULL) {
        /* Handle allocation failure */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    /* Copy values from parameter node to new node */
    strcpy(new_symbol->name, symbol->name);
    new_symbol->type = symbol->type;
    new_symbol->value = symbol->value;
    new_symbol->next = NULL;

    return new_symbol; /* Return newly allocated node */
}

symbol_table* find_symbols_by_type(symbol_table* head, symbol_type type, int* list_size) {
    symbol_table* new_head = NULL; /* Head of the new list. */
    symbol_table* current = head; /* Current symbol in the original list. */
    symbol_table* tmp = NULL;

    *list_size = 0; /* Initialize size to 0. */

    /* Searches list for symbols with given type */
    while (current != NULL) {
        if (current->type == type) {
            /* Add symbol to new table */
            symbol_table* new_symbol = copy_symbol(current);
            tmp = new_head;
            new_head = new_symbol;
            new_head->next = tmp;
            (*list_size)++; /* Increment size. */
        }
        current = current->next;
    }
    return new_head;
}

void free_symbols(symbol_table* head) {
    symbol_table* tmp;

    /* Free nodes in list */
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void add_ic_to_data_symbols(symbol_table* head, unsigned int IC) {
    /* Add IC to all data symbols address */
    while (head != NULL) {
        if (head->type == data_symbol)
            head->value += IC;
        head = head->next;
    }
}