#include "assembler.h"
#include "../lexer/lexer.h"
#include <stdio.h>
#include "../data_structures/vector.h"
#include "../data_structures/trie.h"
#include <string.h>
#include "../preprocessor/preprocessor.h"



#define MAX_LINE_LENGTH 81
#define BASE_ADDR 100


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
    char symbol_name[MAX_LABEL_LENGTH + 1];
};

struct missing_symbol{
    char symbol_name[MAX_LABEL_LENGTH+1];
    unsigned int call_line;
    unsigned int *word;
    unsigned int call_address;
};

struct obj_file{
    Vector code_section;
    Vector data_section;
    Vector symbol_table;
    Trie symbol_table_lookup;
    Vector missing_symbol_table;
    Vector external_calls;
};



struct external_call{
    char external_name[MAX_LABEL_LENGTH + 1];
    Vector call_address;
};

static void * ctor_missing_symbol(const void * copy){
    return memcpy(malloc(sizeof(struct missing_symbol)), copy, sizeof(struct missing_symbol));
}
static void dtor_missing_symbol(void * item){
    free(item);
}
static void * ctor_mem_word(const void * copy){
    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}

static void dtor_mem_word(void * item){
    free(item);
}

static void * ctor_symbol(const void * copy){
    return memcpy(malloc(sizeof(struct symbol)),copy, sizeof(struct symbol));
}

static void dtor_symbol(void * item) {
    free(item);
}


static void * ctor_external_call(const void * copy){
    return memcpy(malloc(sizeof(struct external_call)),copy,sizeof(struct external_call));
}

static void dtor_external_call(void *item){
    struct external_call* external = item;
    vector_destroy(&external->call_address);
    free(external);
}

static void project_assembler_add_external(Vector external_calls, const char * external_name, const unsigned int call_addr){
    void  * const * begin_ex;
    void  * const * end_ex;
    struct external_call new_external = {0};
    VECTOR_FOR_EACH(begin_ex,end_ex,external_calls){
        if(*begin_ex){
            if(strcmp(external_name,((const struct external_call * )(*begin_ex))->external_name) ==0){
                vector_insert(((const struct external_call * )(*begin_ex))->call_address,&call_addr);
                return;
            }
        }
    }
    strcpy(new_external.external_name,external_name);
    new_external.call_address = new_vector(ctor_mem_word,dtor_mem_word);
    vector_insert(new_external.call_address,call_addr);
    vector_insert(external_calls,&new_external);
}
static int project_assembler_compile(FILE * am_file,const char * am_file_name ,  struct obj_file * obj){
    char line_buffer[MAX_LINE_LENGTH] = {0};
    project_ast ast;
    struct missing_symbol missingSymbol = {0};
    struct external_call  externalCall= {0};
    struct symbol local_symbol = {0};
    struct symbol * find_symbol;
    int external_call_addr = 0;
    int line_counter=1;
    unsigned int word;
    int i;
    unsigned int * inserted_word;
    const char * data_str;
    void *const * begin_symbol;
    void * const *end_symbol;
    void * const *begin;
    void * const *end;
    unsigned int *missing_word;
    while (fgets(line_buffer,sizeof(line_buffer),am_file)){
        ast = lexer_get_ast(line_buffer);
        switch (ast.project_ast_options) {
            if(ast.project_ast_options <= project_ast_directive && ast.label_name[0] != '\0'){
                strcpy(local_symbol.symbol_name, ast.label_name);
                find_symbol = trie_exists(obj->symbol_table_lookup, ast.label_name);
                if(ast.project_ast_options == project_ast_option){
                    if(find_symbol){
                        if(find_symbol->symbol_type!=symbol_entry){
                            /*ERRPR REDEF*/
                        } else{
                            find_symbol->symbol_type = symbol_entry_code;
                            find_symbol->address = (unsigned int)vector_get_item_count(obj->code_section) + BASE_ADDR;
                        }
                    } else{
                            local_symbol.symbol_type = symbol_code;
                            local_symbol.address = (unsigned int)vector_get_item_count(obj->code_section) + BASE_ADDR;
                            local_symbol.declared_line=line_counter;
                            trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table,&local_symbol));

                        }
                } else{
                    if(find_symbol){
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options >= project_ast_directive_string){
                            if(find_symbol->symbol_type != symbol_entry){
                                /*ERROR REDEF*/

                            }else{
                                find_symbol->symbol_type = symbol_entry_data;
                            }
                        }
                    }else{
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options >= project_ast_directive_string){
                            local_symbol.symbol_type = symbol_entry_data;
                            local_symbol.address = (unsigned int)vector_get_item_count(obj->data_section);
                            local_symbol.declared_line=line_counter;
                            trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table,&local_symbol));
                        } else{
                            /* WARNING unused label...*/
                        }
                    }
                }
            }
            case project_ast_syntax_error:
            break;
            case project_ast_option: 
                word = ast.directive_or_inst.project_ast_inst_operand_options[1] << 2; /*destination move to bits 2-4*/
                word |= ast.directive_or_inst.project_ast_inst_operand_options[0] << 9; /*source move to bits 11-9*/
                word |= ast.directive_or_inst.project_ast_instant.project_ast_inst_opt << 5; /*opcode move to bits 5-8*/
                vector_insert(obj->code_section,&word); 
                if (ast.directive_or_inst.project_ast_instant.project_ast_inst_opt >= project_ast_inst_mov && 
                 ast.directive_or_inst.project_ast_instant.project_ast_inst_opt <= project_ast_inst_lea){
                    /* mov,cmp,add,sub,lea*/
                    if(ast.directive_or_inst.project_ast_inst_operand_options[1]== project_ast_operand_option_register_number &&
                    ast.directive_or_inst.project_ast_inst_operand_options[0] == project_ast_operand_option_register_number){ /*specific case of two registers*/
                        word = ast.directive_or_inst.project_ast_instant_operands[1].reg_num << 2 ;
                        word |= ast.directive_or_inst.project_ast_instant_operands[0].reg_num << 7 ;
                        vector_insert(obj->code_section,&word);
                    }else{
                        for(i=0;i<2;i++){
                            switch (ast.directive_or_inst.project_ast_inst_operand_options[i]) {
                            case project_ast_operand_option_const_number:  
                            word = ast.directive_or_inst.project_ast_instant_operands[i].const_num << 2;
                            vector_insert(obj->code_section,&word);
                            break;
                            case project_ast_operand_option_register_number:
                            word = ast.directive_or_inst.project_ast_instant_operands[i].const_num << (7-(i*5));
                            vector_insert(obj->code_section,&word);
                            break;
                            case project_ast_operand_option_label:
                                find_symbol = trie_exists(obj->symbol_table_lookup, ast.directive_or_inst.project_ast_instant_operands[i].label);
                                if(find_symbol && find_symbol->symbol_type == symbol_entry){
                                word = find_symbol->address << 2;
                                if(find_symbol->symbol_type == symbol_extern){
                                    word |= 1;
                                    external_call_addr = vector_get_item_count(obj->code_section) + BASE_ADDR;
                                    project_assembler_add_external(obj->external_calls,find_symbol->symbol_name,external_call_addr);
                                }else{
                                    word |= 2;
                                }
                            }
                            inserted_word = vector_insert(obj->code_section,&word);
                            if(!find_symbol || (find_symbol && find_symbol->symbol_type == symbol_entry)){
                                /*symbol might be found later*/
                                strcpy(missingSymbol.symbol_name,ast.directive_or_inst.project_ast_instant_operands[i].label);
                                missingSymbol.word = inserted_word;
                                missingSymbol.call_line=line_counter;
                                missingSymbol.call_address = external_call_addr = vector_get_item_count(obj->code_section) + BASE_ADDR -1;
                                vector_insert(obj->missing_symbol_table,&missingSymbol);
                            }
                            break;
                            case project_ast_operand_option_none:
                                continue;
                            break;
                            }
                            inserted_word = vector_insert(obj->code_section,&word);
                        }
                    }
                } else{
                    /* rts,stop*/
                    /*no need to do anything since its only one word*/
                }
            break;
            case project_ast_directive:
                switch (ast.directive_or_inst.project_ast_dir.project_ast_dir_options){
                    case project_ast_directive_data:
                        for(i=0;data_str - ast.directive_or_inst.project_ast_dir.dir_operands.data.dataCount;i++){
                            vector_insert(obj->data_section,&ast.directive_or_inst.project_ast_dir.dir_operands.data.data[i]);
                        }
                    break;
                    case project_ast_directive_string:
                        for(i=0,data_str - ast.directive_or_inst.project_ast_dir.dir_operands.string;*data_str;data_str++){
                            word= *data_str;
                            vector_insert(obj->data_section,&word);
                        }
                    word=0;
                    vector_insert(obj->data_section,&word);
                    break;
                break;
                case project_ast_directive_external: 
                    find_symbol=trie_exists(obj->symbol_table_lookup,ast.directive_or_inst.project_ast_dir.dir_operands.labelName);
                    if(find_symbol){
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options == project_ast_directive_entry){
                            if(find_symbol->symbol_type==symbol_entry || find_symbol->symbol_type == symbol_entry_code || find_symbol->symbol_type == symbol_entry_data){
                            /* WARNING REDEF*/ 

                            }else if(find_symbol->symbol_type == symbol_extern){
                                /*ERROR was declared as external but now is declared as entry?*/
                            }else{
                                if(find_symbol->symbol_type == symbol_code){
                                    find_symbol->symbol_type =symbol_entry_code;
                                }
                                else{
                                    find_symbol->symbol_type=symbol_entry_data;
                                }
                            }
                        }else{ /*if current line is external*/
                            if(find_symbol->symbol_type==symbol_extern){
                                /*WARNING REDEFINE*/

                            }else{
                            /*ERROR was declared as {PULL FROM STRUCT} and now being declared as external*/
                            }
                        }
                    } else{
                        strcpy(local_symbol.symbol_name,ast.directive_or_inst.project_ast_dir.dir_operands.labelName);
                        local_symbol.symbol_type = ast.directive_or_inst.project_ast_dir.project_ast_dir_options;
                        local_symbol.declared_line=line_counter;
                        local_symbol.address = 0;
                        trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table,&local_symbol));
                    }
                break;
                case project_ast_directive_entry:
                find_symbol=trie_exists(obj->symbol_table_lookup,ast.directive_or_inst.project_ast_dir.dir_operands.labelName);
                    if(find_symbol){
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options == project_ast_directive_entry){
                            if(find_symbol->symbol_type==symbol_entry || find_symbol->symbol_type >= symbol_entry_code){
                            /* WARNING REDEF*/ 

                            }else if(find_symbol->symbol_type == symbol_extern){
                                /*ERROR was declared as external but now is declared as entry?*/
                            }else{
                                if(find_symbol->symbol_type == symbol_code){
                                    find_symbol->symbol_type =symbol_entry_code;
                                }
                                else{
                                    find_symbol->symbol_type=symbol_entry_data;
                                }
                            }
                        }else{ /*if current line is external*/
                            if(find_symbol->symbol_type==symbol_extern){
                                /*WARNING REDEFINE*/

                            }else{
                            /*ERROR was declared as {PULL FROM STRUCT} and now being declared as external*/
                            }
                        }
                    } else{
                        strcpy(local_symbol.symbol_name,ast.directive_or_inst.project_ast_dir.dir_operands.labelName);
                        local_symbol.symbol_type = ast.directive_or_inst.project_ast_dir.project_ast_dir_options;
                        local_symbol.declared_line=line_counter;
                        trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table,&local_symbol));
                    }

                break;
            }
        }
        line_counter++;
    }
    VECTOR_FOR_EACH(begin_symbol,end_symbol,obj->symbol_table){
        if(*begin_symbol){
            if( ((struct symbol* )(*begin_symbol))->symbol_type == symbol_entry ||
             ((struct symbol* )(*begin_symbol))->symbol_type == symbol_entry_code){
                /* ERROR entry X was declared at line Y but was never defined in file*/
            }
        }
    }
    VECTOR_FOR_EACH(begin,end,obj->missing_symbol_table){
        if(*begin){
            find_symbol = trie_exists(obj->symbol_table_lookup,((struct missing_symbol *)(*begin))->symbol_name);
            if(find_symbol && find_symbol->symbol_type!=symbol_entry){
                *(((struct missing_symbol *)(*begin))->word) = find_symbol->address << 2;
                if(find_symbol->symbol_type==symbol_extern){
                     *(((struct missing_symbol *)(*begin))->word) |= 1;
                     project_assembler_add_external(obj->external_calls,find_symbol->symbol_name,(((struct missing_symbol *)(*begin))->call_address));
                }else{
                     *(((struct missing_symbol *)(*begin))->word) |= 2;
                }
            }else{
                /* ERROR missing label, calling label at line X and it was not found in the symbol table*/
            }
        }
    }
}

static struct obj_file project_assembler_create_job(){
    struct obj_file ret;
    ret.code_section = new_vector(ctor_mem_word,dtor_mem_word);
    ret.data_section = new_vector(ctor_mem_word,dtor_mem_word);
    ret.symbol_table = new_vector(ctor_symbol,dtor_symbol);
    ret.external_calls = new_vector(ctor_external_call,dtor_external_call);
    ret.symbol_table_lookup = trie();
    return ret;
}

static void project_assembler_destroy_obj(struct obj_file * obj){
    vector_destroy(&obj->code_section);
    vector_destroy(&obj->data_section);
    vector_destroy(&obj->symbol_table);
    vector_destroy(&obj->external_calls);
    trie_destroy(&obj->symbol_table_lookup);
};

int project_assembler(int* file_counter, char **file_names){
    int i;
    const char * am_file_name;
    FILE * am_file;
    struct obj_file curr_obj;
    for(i=0;i<file_counter;file_names++){
        am_file_name = project_preprocess(file_names[i]);
        am_file = fopen(am_file_name,"r"); /* open file with read only permissions*/
        if(am_file_name && am_file){ /* preprocessor finished successfully*/
            curr_obj=project_assembler_create_job();
            if(project_assembler_compile(am_file,&curr_obj,am_file_name)==1){ /*if compilation was successfull...*/

            }
            fclose(am_file);/*close the file*/
            free((void*)am_file_name);/*free the memory on the am_file*/
            project_assembler_destroy_obj(&curr_obj);/*destory the object*/
        }
    }
    return 0;
}