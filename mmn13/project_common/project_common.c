#include "project_common.h"
#include <string.h>
#include <stdlib.h>

const char * symbol_type[6] = {
    "external symbol",
    "entry symbol",
    "code symbol",
    "data symbol",
    "entry code symbol",
    "entry data symbol"
};

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

void project_assembler_add_external(Vector external_calls, const char * external_name, const unsigned int call_addr){
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
    vector_insert(new_external.call_address,&call_addr);
    vector_insert(external_calls,&new_external);
}

struct obj_file project_assembler_create_job(){
    struct obj_file ret = {0};
    ret.code_section = new_vector(ctor_mem_word,dtor_mem_word);
    ret.data_section = new_vector(ctor_mem_word,dtor_mem_word);
    ret.symbol_table = new_vector(ctor_symbol,dtor_symbol);
    ret.external_calls = new_vector(ctor_external_call,dtor_external_call);
    ret.symbol_table_lookup = trie();
    return ret;
}

void project_assembler_destroy_obj(struct obj_file * obj){
    vector_destroy(&obj->code_section);
    vector_destroy(&obj->data_section);
    vector_destroy(&obj->symbol_table);
    vector_destroy(&obj->external_calls);
    trie_destroy(&obj->symbol_table_lookup);
}