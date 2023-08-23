#include "assembler.h"
#include "../lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include "../data_structures/vector.h"
#include "../data_structures/trie.h"
#include <string.h>
#include "../preprocessor/preprocessor.h"
#include "../project_common/project_common.h"
#include "../output_unit/output_unit.h"
#include <stdarg.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 81
#define BASE_ADDR 100
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"


struct missing_symbol{
    char symbol_name[MAX_LABEL_LENGTH+1];
    unsigned int call_line;
    unsigned int *word;
    unsigned int call_address;
};
static void * ctor_missing_symbol(const void * copy){
    return memcpy(malloc(sizeof(struct missing_symbol)), copy, sizeof(struct missing_symbol));
}

static void dtor_missing_symbol(void * item){
    free(item);
}

static void project_assembler_warning(const char * file_name, int line, const char * fmt,...){
    va_list vl;
    va_start(vl,fmt);
    printf("%s:%d " ANSI_COLOR_YELLOW "warning: " ANSI_COLOR_RESET,file_name,line);
    vprintf(fmt,vl);
    printf("\n");
    va_end(vl);
}
static void project_assembler_error(const char * file_name, int line, const char * fmt,...){
    va_list vl;
    va_start(vl,fmt);
    printf("%s:%d " ANSI_COLOR_RED "ERROR: " ANSI_COLOR_RESET,file_name,line);
    vprintf(fmt,vl);
    printf("\n");
    va_end(vl);
}
static int project_assembler_compile(FILE * am_file,const char * am_file_name ,  struct obj_file * obj){
    char line_buffer[MAX_LINE_LENGTH] = {0};
    project_ast ast;
    struct symbol local_symbol = {0};
    struct symbol * find_symbol = NULL;
    struct missing_symbol missingSymbol;
    int external_call_addr = 0;
    int line_counter=1;
    unsigned int word;
    int i;
    unsigned int * inserted_word;
    const char * data_str;
    void *const * begin_symbol;
    void * const *end_symbol;
    void * const *begin = NULL;
    void * const *end;
    int errorCode = 1;
    Vector missing_symbol_table = new_vector(ctor_missing_symbol,dtor_missing_symbol);
    while (fgets(line_buffer,sizeof(line_buffer),am_file)){
        ast = lexer_get_ast(line_buffer);
            if(ast.ast_error_msg[0]!='\0'){
                project_assembler_error(am_file_name,line_counter,"%s",ast.ast_error_msg);
                errorCode = 1;
                line_counter++;
                continue;
            }
            if(ast.label_name[0] != '\0'){
            strcpy(local_symbol.symbol_name, ast.label_name);
            find_symbol = trie_exists(obj->symbol_table_lookup, ast.label_name);
            if(ast.project_ast_options == project_ast_option){
                if(find_symbol){
                    if(find_symbol->symbol_type!=symbol_entry){
                        /*ERRPR REDEF*/
                        project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
                        errorCode = 0;
                    } else{
                        find_symbol->symbol_type = symbol_entry_code;
                        find_symbol->address = ((unsigned int)vector_get_item_count(obj->code_section) + BASE_ADDR);
                    }
                } else{
                        local_symbol.symbol_type = symbol_code;
                        local_symbol.address = ((unsigned int)vector_get_item_count(obj->code_section) + BASE_ADDR);
                        local_symbol.declared_line=line_counter;
                        trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table , &local_symbol));

                    }
            } else{
                if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options <= project_ast_directive_entry){
                    /*WARNING neglecting label of extern and entry... it has no meaning*/
                    project_assembler_warning(am_file_name,line_counter,"neglecting label: '%s' since its '%s' ",ast.label_name,symbol_type[ast.directive_or_inst.project_ast_dir.project_ast_dir_options]);
                }
                else{
                    if(find_symbol){
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options >= project_ast_directive_string){
                            if(find_symbol->symbol_type != symbol_entry){
                                /*ERROR REDEF*/
                                project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
                                errorCode = 0;
                            }else{
                                find_symbol->symbol_type = symbol_entry_data;
                                find_symbol->address = vector_get_item_count(obj->data_section);
                                find_symbol->declared_line = line_counter;
                            }
                        }
                    }else{
                        if(ast.directive_or_inst.project_ast_dir.project_ast_dir_options >= project_ast_directive_string){
                            local_symbol.symbol_type = symbol_data;
                            local_symbol.address = (unsigned int)vector_get_item_count(obj->data_section);
                            local_symbol.declared_line=line_counter;
                            trie_insert(obj->symbol_table_lookup, local_symbol.symbol_name, vector_insert(obj->symbol_table,&local_symbol));
                        }
                    }
                }
            }
        }
        switch (ast.project_ast_options) {

            case project_ast_option:
                word = ast.directive_or_inst.project_ast_instant.project_ast_inst_operand_options[1] << 2; /*destination move to bits 2-4*/
                word |= ast.directive_or_inst.project_ast_instant.project_ast_inst_operand_options[0] << 9; /*source move to bits 11-9*/
                word |= ast.directive_or_inst.project_ast_instant.project_ast_inst_opt << 5; /*opcode move to bits 5-8*/
                vector_insert(obj->code_section,&word); 
                if (ast.directive_or_inst.project_ast_instant.project_ast_inst_opt >= project_ast_inst_mov && 
                 ast.directive_or_inst.project_ast_instant.project_ast_inst_opt <= project_ast_inst_lea){
                    /* mov,cmp,add,sub,lea*/
                    if(ast.directive_or_inst.project_ast_instant.project_ast_inst_operand_options[1]== project_ast_operand_option_register_number &&
                    ast.directive_or_inst.project_ast_instant.project_ast_inst_operand_options[0] == project_ast_operand_option_register_number){ /*specific case of two registers*/
                        word = ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[1].reg_num << 2 ;
                        word |= ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[0].reg_num << 7 ;
                        vector_insert(obj->code_section,&word);
                    }else{
                        for(i=0;i<2;i++){
                            switch (ast.directive_or_inst.project_ast_instant.project_ast_inst_operand_options[i]) {
                            case project_ast_operand_option_const_number:  
                            word = ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[i].const_num << 2;
                            vector_insert(obj->code_section,&word);
                            break;
                            case project_ast_operand_option_register_number:
                            word = ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[i].const_num << (7-(i*5));
                            vector_insert(obj->code_section,&word);
                            break;
                            case project_ast_operand_option_label:
                                find_symbol = trie_exists(obj->symbol_table_lookup, ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[i].label);
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
                                strcpy(missingSymbol.symbol_name,ast.directive_or_inst.project_ast_instant.project_ast_instant_operands[i].label);
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
                if((ast.directive_or_inst.project_ast_dir.project_ast_dir_options <= project_ast_directive_data ) 
                && (ast.directive_or_inst.project_ast_dir.project_ast_dir_options <= project_ast_directive_string)
                && (ast.label_name[0] == '\0')){
                   project_assembler_warning(am_file_name,line_counter,"neglecting %s directive since it has no label.",ast.directive_or_inst.project_ast_dir.project_ast_dir_options == project_ast_directive_data ? ".data" : ".string");
                   break; 
                }
                switch (ast.directive_or_inst.project_ast_dir.project_ast_dir_options){
                    case project_ast_directive_data:
                        for(i=0;data_str - ast.directive_or_inst.project_ast_dir.dir_operands.data.dataCount;i++){
                            vector_insert(obj->data_section,&ast.directive_or_inst.project_ast_dir.dir_operands.data.data[i]);
                        }
                    break;
                    case project_ast_directive_string:
                        for(i=0,data_str = ast.directive_or_inst.project_ast_dir.dir_operands.string;*data_str;data_str++){
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
                            project_assembler_warning(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->declared_line);
                            }else if(find_symbol->symbol_type == symbol_extern){
                                /*ERROR was declared as external but now is declared as entry?*/
                                project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->declared_line);
                                errorCode = 0;
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
                                project_assembler_warning(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->declared_line);
                            }else{
                                /*ERROR was declared as {PULL FROM STRUCT} and now being declared as external*/
                                project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->declared_line);
                                errorCode = 0;
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
                            project_assembler_warning(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
                            }else if(find_symbol->symbol_type == symbol_extern){
                                /*ERROR was declared as external but now is declared as entry?*/
                                project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
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
                                project_assembler_warning(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
                            }else{
                                /*ERROR was declared as {PULL FROM STRUCT} and now being declared as external*/
                                project_assembler_error(am_file_name,line_counter,"label: '%s' was already defined as '%s' in line: '%d' . ",find_symbol->symbol_name,symbol_type[find_symbol->symbol_type], find_symbol->address);
                            }
                        }
                    } else{
                        strcpy(local_symbol.symbol_name,ast.directive_or_inst.project_ast_dir.dir_operands.labelName);
                        local_symbol.symbol_type = ast.directive_or_inst.project_ast_dir.project_ast_dir_options;
                        local_symbol.address = 0;
                        local_symbol.declared_line = line_counter;
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
            if(((struct symbol* )(*begin_symbol))->symbol_type == symbol_entry){
                /* ERROR entry X was declared at line Y but was never defined in file*/
                project_assembler_error(am_file_name,line_counter,"label: '%s':'%s' was declared in line '%d' but was never defined in this file.",((struct symbol *)(*begin))->symbol_name, symbol_type[symbol_entry],((struct symbol *)(*begin))->declared_line);
                errorCode = 0;
            }else if(((struct symbol*)(*begin))->symbol_type >= symbol_entry_code){
                obj->entries_count++;
            }
            if(((struct symbol*)(*begin))->symbol_type == symbol_entry_data || ((struct symbol*)(*begin))->symbol_type == symbol_data){
                ((struct symbol*)(*begin))-> address += vector_get_item_count(obj->code_section) + BASE_ADDR;
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
                project_assembler_error(am_file_name,line_counter,"missing label:'%s' was refered in line: '%d' but was never defined in this file.",(((struct missing_symbol *)(*begin))->symbol_name),(((struct missing_symbol *)(*begin))->call_line));
                errorCode = 0;
            }
        }
    }
    vector_destroy(&missing_symbol_table);
    return errorCode;
}





int project_assembler(int file_counter, char **file_names){
    int i;
    const char * am_file_name;
    FILE * am_file;
    struct obj_file curr_obj;
    const char * out_dir = NULL;
    for(i=0;i<file_counter;i++){
        if(file_names[i] == NULL) continue;
        if(strcmp(file_names[i],"-o")==0){
            out_dir = file_names[i+1];
            file_names[i] = NULL;
            file_names[i+1] = NULL;
        }
    }
    for(i=0;i<file_counter;i++){
        if(file_names == NULL) continue;
        am_file_name = project_preprocess(file_names[i],out_dir);
        if(am_file_name){ /* preprocessor finished successfully*/
            am_file = fopen(am_file_name,"r"); /* open file with read only permissions*/
            if(am_file_name){
                curr_obj=project_assembler_create_job();
                if(project_assembler_compile(am_file,am_file_name,&curr_obj)==1){ /*if compilation was successfull...*/
                    projectOutput(file_names[i],&curr_obj,out_dir);
                }
                fclose(am_file);
                project_assembler_destroy_obj(&curr_obj);/*destory the object*/
            }
            free((void*)am_file_name);/*free the memory on the am_file*/
        }
    }
    return 0;
}