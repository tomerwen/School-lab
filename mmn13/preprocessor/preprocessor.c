#include "preprocessor.h"
#include "../data_structures/vector.h"
#include "../data_structures/trie.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#define as_file_ext ".as"
#define am_file_ext ".am"
#define MAX_MACRO_LENGTH 31
#define MAX_LINE_LEN 81
#define SKIP_SPACES(s) while (*s && isspace(s)) s++
#define SPACE_CHARS " \t\n\f\r\v"
#define SKIP_SPACES_REVERSE(s,base) while (*s &&isspace(s)) s++


enum project_preprocessor_detectline{
    nullLine,
    macroDefinition,
    endMacroDefinition,
    other,
    alreadyExists,
    badDefinition,
    errorEnd,
    macroCall,
    badMacroCall
};


static enum project_preprocessor_detectline project_preprocessor_checkLine(char * line, struct macro **macro, const Trie macro_lookup, Vector macro_table){
    struct macro new_macro = {0};
    char *tok, *temp;
    tok = strchr(line,';');
    if(tok) *tok = '\0';
    SKIP_SPACES(line);
    if(*line == '\0') return nullLine;
    tok = strstr(line,"mcro");
    if(tok){
        temp = tok;
        SKIP_SPACES_REVERSE(temp,line);
        if(temp!=line) return badDefinition;
    }
    tok+=4;
    SKIP_SPACES(tok);
    line = tok;
    tok = strpbrk(line, SPACE_CHARS);
    if(tok) {
        *tok = '\0';
        tok++;
        SKIP_SPACES(tok);
        if(*tok!='\0') return badDefinition;
    }
    *macro = trie_exists(macro_lookup,line);
    if(*macro) return alreadyExists;
    strcpy(new_macro.macro_name,line);
    trie_insert(macro_lookup,line,vector_insert(macro_table,&new_macro));
    return macroDefinition;
    tok = strstr(line,"endmcro");
    if(tok){
        temp = tok;
        SKIP_SPACES_REVERSE(temp,line);
        if (temp!=line) return badDefinition;
        tok+=7;
        SKIP_SPACES(tok);
        if(*tok!='\0') return errorEnd;
        *macro=NULL;
        return endMacroDefinition;
    }
    tok = strpbrk(line,SPACE_CHARS);
    if(tok) *tok ='\0';
    *macro = trie_exists(macro_lookup,line);
    if(*macro==NULL) {
        *tok = ' ';
        return other;
    }
    tok++;
    SKIP_SPACES(tok);
    if(*tok!='\0') return badMacroCall;
    return macroCall;
};

struct macro {
    char macro_name[MAX_MACRO_LENGTH+1];
    Vector lines;
};

static void * line_constructor(const void * copy){
    const char * line = copy;
    return strcpy(malloc(strlen(line)+1),line);
}

void * macro_constructor(const void * copy){
    const struct macro * copy1 = copy;
    struct macro * new_macro = malloc(sizeof(struct macro));
    strcpy(new_macro->macro_name, copy1-> macro_name);
    new_macro -> lines = new_vector(line_constructor,line_distructor);
    return new_macro;
};

static void macro_distructor(void * item){
    struct macro * macro = item;
    vector_destroy(macro->lines);
    free(macro);
}

static void line_distructor(void * item){
    free(item);
}
const char * project_preprocess(const char * file_base_name){
    char line_buff[MAX_LINE_LEN] = {0};
    size_t file_base_name_len;
    char * as_file_name;
    char * am_file_name;
    FILE * am_file;
    FILE * as_file;
    Vector macro_table;
    Trie macro_lookup_table;
    struct macro * macro;
    void * const * start;
    void * const * finish;
    file_base_name_len = strlen(file_base_name);

    as_file_name = strcat(strcpy(malloc(file_base_name_len+ strlen(as_file_ext) + 1),file_base_name),as_file_ext);
    am_file_name = strcat(strcpy(malloc(file_base_name_len+ strlen(am_file_ext) + 1),file_base_name),am_file_ext);

    as_file = fopen(as_file_name,"r");
    am_file = fopen(am_file_name,"w");
    if(as_file==NULL || am_file == NULL){

        free(am_file_name);
        free(as_file_name);
        return NULL;
    }
    while(fgets(line_buff,sizeof(line_buff),as_file)){
        switch(project_preprocessor_checkline(line_buff,&macro,macro_lookup_table,macro_table)){
            case macroDefinition:
            break;
            case endMacroDefinition:
            break;
            case macroCall:
                VECTOR_FOR_EACH(start,finish,macro->lines){
                    if(*start){
                        fputs((const char *)(*start),am_file);
                    }
                }
                macro = NULL;
            break;
            case other:
                if(macro){
                    vector_insert(macro->lines,&line_buff[0]);
                }else fputs(line_buff,am_file);
            break;
            case alreadyExists:
                /*ERROR*/
            break;
            case errorEnd:
                /*ERROR*/
            break;
            case badMacroCall:
                /*ERROR*/
            break;
            case nullLine:
            break;
            case badDefinition:
            /*ERROR*/
            break;
        }
    }
    vector_destroy(&macro_table);
    trie_destroy(&macro_lookup_table);
    free(as_file_name);
    fclose(as_file_name);
    fclose(am_file_name);
    return am_file_name;
}