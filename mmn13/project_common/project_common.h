#ifndef __PROJECT_COMMON_H_
#define __PROJECT_COMMON_H_


#include "../data_structures/vector.h"
#include "../data_structures/trie.h"



struct symbol{
    enum{
        symbol_extern,
        symbol_entry,
        symbol_code,
        symbol_data,
        symbol_entry_code,
        symbol_entry_data
        /*if symbol is symbol_entry that means that it must be defined later in the file*/
    }symbol_type;
    int address;
    unsigned int declared_line;
    char symbol_name[31 + 1];
};


struct obj_file{
    Vector code_section;
    Vector data_section;
    Vector symbol_table;
    Trie symbol_table_lookup;
    Vector missing_symbol_table;
    Vector external_calls;
    int entries_count;
};
#endif