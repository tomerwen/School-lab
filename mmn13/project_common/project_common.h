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
extern const char * symbol_type[6];

struct obj_file{
    Vector code_section;
    Vector data_section;
    Vector symbol_table;
    Trie symbol_table_lookup;
    Vector missing_symbol_table;
    Vector external_calls;
    int entries_count;
};

struct external_call{
    char external_name[31 + 1];
    Vector call_address;
};

struct obj_file project_assembler_create_job();
void project_assembler_destroy_obj(struct obj_file * obj);
void project_assembler_add_external(Vector external_calls, const char * external_name, const unsigned int call_addr);
#endif