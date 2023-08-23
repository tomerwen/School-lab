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

static char* basename(char* path) {
    char* base = strrchr(path, '/');
    return base ? base + 1 : path;
}

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

struct macro {
    char macro_name[MAX_MACRO_LENGTH+1];
    Vector lines;
};

static void * line_constructor(const void * copy){
    const char * line = copy;
    return strcpy(malloc(strlen(line)+1),line);
}



static void line_distructor(void * item){
    free(item);
}
void * macro_constructor(const void * copy){
    const struct macro * copy1 = copy;
    struct macro * new_macro = malloc(sizeof(struct macro));
    strcpy(new_macro->macro_name, copy1-> macro_name);
    new_macro -> lines = new_vector(line_constructor,line_distructor);
    return new_macro;
}
static void macro_distructor(void * item){
    struct macro * macro = item;
    vector_destroy(&macro->lines);
    free(macro);
}
static enum project_preprocessor_detectline project_preprocessor_checkLine(char * line, struct macro **macro, const Trie macro_lookup, Vector macro_table){
    struct macro new_macro = {0};
    struct macro * local;
    char *tok, *temp;
    tok = strchr(line,';');
    if(tok) *tok = '\0';
    SKIP_SPACES(line);
    if(*line == '\0') return nullLine;
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
    tok = strstr(line,"mcro");
    if(tok){
        temp = tok;
        SKIP_SPACES_REVERSE(temp,line);
        if(temp!=line) return badDefinition;
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
        *macro = vector_insert(macro_table,&new_macro);
        trie_insert(macro_lookup,line,vector_insert(macro_table,&new_macro));
        return macroDefinition;
    }
    
    tok = strpbrk(line,SPACE_CHARS);
    if(tok) *tok ='\0';
    local = trie_exists(macro_lookup,line);
    if(local==NULL) {
        *tok = ' ';
        return other;
    }
    tok++;
    SKIP_SPACES(tok);
    if(*tok!='\0') return badMacroCall;
    *macro = local;
    return macroCall;
}
const char * project_preprocess(char * file_base_name, const char * out_dir){
    char line_buff[MAX_LINE_LEN] = {0};
    enum project_preprocessor_detectline mpld;
    size_t file_base_name_len, am_file_name_base_len;
    char * as_file_name;
    char * am_file_name;
    FILE * am_file;
    FILE * as_file;
    Vector macro_table = NULL;
    Trie macro_lookup_table = NULL;
    struct macro * macro = NULL;
    void * const * start;
    void * const * finish;
    int line_count = 1;
    file_base_name_len = strlen(file_base_name);

    as_file_name = strcat(strcpy(malloc(file_base_name_len+ strlen(as_file_ext) + 1),file_base_name),as_file_ext);
    file_base_name = out_dir != NULL ? basename((char *)file_base_name) : file_base_name;
    file_base_name = strcat(strcpy(malloc(strlen(out_dir)+strlen(file_base_name)+1),out_dir),file_base_name);
    am_file_name_base_len = strlen(file_base_name);
    am_file_name = strcat(strcpy(malloc(am_file_name_base_len + strlen(am_file_ext) + 1),file_base_name),am_file_ext);

    as_file = fopen(as_file_name,"r");
    if(as_file == NULL){
        if(out_dir) free(file_base_name);
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }
    am_file = fopen(am_file_name,"w");
    if(as_file==NULL){
        if(out_dir) free(file_base_name);
        free(as_file_name);
        free(am_file_name);
        return NULL;
    }
    if(am_file == NULL){

        free(am_file_name);
        free(as_file_name);
        return NULL;
    }
    macro_table = new_vector(macro_constructor,macro_distructor);
    macro_lookup_table = trie();
    while(fgets(line_buff,sizeof(line_buff),as_file)){
        switch(mpld = project_preprocessor_checkLine(line_buff,&macro,macro_lookup_table,macro_table)){
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
        line_count++;
    }
    vector_destroy(&macro_table);
    trie_destroy(&macro_lookup_table);
    free(as_file_name);
    fclose(as_file);
    fclose(am_file);
    if(out_dir) free(file_base_name);
    return am_file_name;
}