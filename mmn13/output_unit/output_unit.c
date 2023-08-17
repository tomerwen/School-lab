#include "output_unit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define EXT_EXTENSION ".ext"
#define ENT_EXTENSION ".ent"
#define OB_EXTENSION ".ob"
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqurstuvwxyz123456789+/"
static void project_output_ent_file(const char * ent_file_name, Vector symbol_table){
    FILE * ent_file;
    void * const * begin;
    void * const * end;
    ent_file = fopen(ent_file_name,"w");
    if(ent_file_name){
        VECTOR_FOR_EACH(begin,end,symbol_table);
        if(*begin){
            if(((struct symbol*)(*begin))->symbol_type >= symbol_entry_code){
                fprintf(ent_file,"%s\t%u",((struct symbol*)(*begin))->symbol_name,((struct symbol*)(*begin))->address);
            }
        }
        fclose(ent_file);
    }
}

static void project_output_ext_file(const char * ext_name, Vector extern_calls){
    FILE * ext_file;
    void * const * begin;
    void * const * end;
    void *const * begin_addr;
    void *const * end_addr;
    ext_file = fopen(ext_name,"w");
    if(ext_file){
        VECTOR_FOR_EACH(begin,end,extern_calls){
            if(*begin){
                VECTOR_FOR_EACH(begin_addr,end_addr,((const struct external_call *)(*begin))->call_address){
                    if(*begin_addr){
                        fprintf(ext_file, "%s\t%u", ((const struct external_call *)(*begin))->external_name, *((const unsigned int *)(*begin_addr)));
                    }
                }
            }
        }
    }
    fclose(ext_file);
}


void project_output_memory_section(FILE * file, const Vector mem_sec) {
    void*const * begin;
    void *const * end;
    unsigned int msb;
    unsigned int lsb;
    const char * const b64_chars = BASE64;
    int i;
    VECTOR_FOR_EACH(begin, end, mem_sec) {
        if (*begin) {
            msb= (*(unsigned int *)(*begin) >> 6) &0x3F;
            lsb = (*(unsigned int *)(*begin) & 0x3F);
            fprintf(file,"%c%c", b64_chars[msb],b64_chars[lsb]);
            fprintf(file,"%s","\n");
        }
    }
}
void projectOutput(char * name, const struct obj_file *obj, const char *out_dir) {
    size_t str_len;
    char *ext_file_name;
    char *ent_file_name;
    char *ob_file_name;
    FILE *ob_file;
    name = out_dir != NULL ? basename((char *)name) : name;
    name = strcat(strcpy(malloc(strlen(out_dir)+strlen(name)+1),out_dir),name);
    str_len = strlen(name);
    if (obj->entries_count >= 1) {
        ent_file_name = strcat(strcpy(malloc(str_len + strlen(ENT_EXTENSION) + 1), name), ENT_EXTENSION);
        project_output_ent_file(ent_file_name, obj->symbol_table);
        free(ent_file_name);
    }

    if (vector_get_item_count(obj->external_calls) >= 1) {
        ext_file_name = strcat(strcpy(malloc(str_len + strlen(EXT_EXTENSION) + 1), name), EXT_EXTENSION);
        project_output_ext_file(ext_file_name, obj->external_calls);
        free(ext_file_name);
    }

    ob_file_name = strcat(strcpy(malloc(str_len + strlen(OB_EXTENSION) + 1), name), OB_EXTENSION);
    ob_file = fopen(ob_file_name, "w");
    if (ob_file) {
        fprintf(ob_file, "%lu %lu \n", (unsigned long)vector_get_item_count(obj->code_section), (unsigned long)vector_get_item_count(obj->data_section));
        project_output_memory_section(ob_file, obj->code_section);
        fclose(ob_file);
    }
    free(ob_file_name);
    if(out_dir) free(name);
}