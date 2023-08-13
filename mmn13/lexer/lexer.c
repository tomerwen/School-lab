#include "lexer.h"
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../data_structures/trie.h"

#define SKIP_SPACES(s) while (*s && isspace(s)) s++
#define SPACE_CHARS " \t\n\f\r\v"
#define SKIP_SPACES_REVERSE(s,base) while (*s &&isspace(s)) s++
#define MAX_REG 7
#define MIN_REG 0
#define MAX_CONST_NUM 511
#define MIN_CONST_NUM 512


static int is_trie_init = 0;
Trie inst_lookup = NULL;
Trie dir_lookup = NULL;
static struct  asm_inst_dict{
    const char *inst_name;
    int key;
    const char *source_op_options; /* Myiadi(1), Yashir(3), Register(5)*/
    const char *dest_op_options;
} asm_inst_dict[16] = {
    [0] = {"mov",project_ast_inst_mov,"MYR","YR"},
    [1] = {"cmp",project_ast_inst_cmp,"MYR","MYR"},
    [2] = {"add",project_ast_inst_add,"MYR","YR"},
    [3] = {"sub",project_ast_inst_sub,"MYR","YR"},
    [4] = {"lea",project_ast_inst_lea,"Y","YR"},
    [5] = {"not",project_ast_inst_not,NULL,"YR"},
    [6] = {"clr",project_ast_inst_clr,NULL,"YR"},
    [7] = {"inc",project_ast_inst_inc,NULL,"YR"},
    [8] = {"dec",project_ast_inst_dec,NULL,"YR"},
    [9] = {"jmp",project_ast_inst_jmp,NULL,"YR"},
    [10] = {"bne",project_ast_inst_bne,NULL,"YR"},
    [11] = {"red",project_ast_inst_red,NULL,"YR"},
    [12] = {"prn",project_ast_inst_prn,NULL,"MYR"},
    [13] = {"jsr",project_ast_inst_jsr,NULL,"YR"},
    [14] = {"rts",project_ast_inst_rts,NULL,NULL},
    [15] = {"stop",project_ast_inst_stop,NULL,NULL}
};

static struct asm_dir{
    const char * name;
    int key;
} asm_dir[4] = {
    [0]= {"data",project_ast_directive_data},
    [1]= {"entry",project_ast_directive_entry},
    [2]= {"extern",project_ast_directive_external},
    [3]= {"string",project_ast_directive_string}
};


static void lexer_trie_init(){
    int i;
    inst_lookup = trie();
    for(i=0;i<16;i++) trie_insert(inst_lookup,asm_inst_dict[i].inst_name,&asm_inst_dict[i]);
    for(i=0;i<4;i++) trie_insert(dir_lookup,asm_dir[i].name,&asm_dir[i]);
    is_trie_init=1;
}

enum valid_err{
    ok,
    startWithoutAlpha,
    containsNoAlpha,
    tooLong
};

static enum valid_err lexer_is_valid(const char *label){
    int counter = 0;
    if(!isalpha(*label)) return startWithoutAlpha;
    label++;
    while(*label && isalpha(*label)){
        counter++;
        label++;
    };
    if(*label!='\0') return containsNoAlpha;
    if(counter>MAX_LABEL_LENGTH) return tooLong;
    return ok; 
}

static int lexerParseNum(const char *num_str, char ** endptr, long * num, long min, long max){
    errno =0;
    char * my_end;
    *num = strtol(num_str,&my_end,10);
    while(isspace(*my_end)) my_end++;
    if(*endptr !='\0') return -1;
    if(errno==ERANGE) return -2;
    if(*num>max || *num < min) return -3;
    if(endptr) *endptr=my_end;
    return 0;
}
static char lexerParseOperandI(char * operandString, char ** label, int * constNumber, int * regNumber){
    SKIP_SPACES(operandString);
    char * temp;
    long num;
    if(*operandString == '@') {
        if((*operandString+1)=='r'){
            if(*(operandString+2) == '+' || *(operandString+2) == '-') return 'N';
            if(lexerParseNum(operandString+2,NULL,&num,MIN_REG,MAX_REG)!=0) return 'N';
            *regNumber = (int)num;
        }
        return 'N';
    }
    if (lexerParseNum(operandString,NULL,&num,MIN_CONST_NUM,MAX_CONST_NUM)){

    }
}


static void lexerParseInstructionOperands(project_ast * ast, char * operandString,struct  asm_inst_dict *aib){
    int counter = 0;
    char operandi;
    char * tok1;
    char * seperator = strchr(operandString,',');
    if(seperator){
        tok1 = strchr(seperator+1,',');
        if(tok1) strcpy(ast->ast_error_msg,"found at least two seperator tokens");
        if(aib->source_op_options==NULL) {
            strcpy(ast->ast_error_msg,"instruction: %s expects only 1 operand but recieved more", aib->inst_name);
            return;
        }
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant_operands[0].reg_num);
        if(operandi == 'N'){
            sprintf(ast->ast_error_msg,"Unkown keyword: %s",operandString);
            return;
        }
        if(strchr(aib->source_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"source operand: %s is not supported", operandString);
            return;
        }
        operandString = seperator+1;
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant_operands[0].reg_num);
        if(operandi == 'N'){
            sprintf(ast->ast_error_msg,"Unkown operand: for destination %s",operandString);
            return;
        }
        if(strchr(aib->dest_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"destination operand: %s is not supported", operandString);
            return;
        }
    }else{
        if(aib->source_op_options!=NULL){
            strcpy(ast->ast_error_msg,"instruction: %s expects seperator token ','", aib->inst_name);
            return;
        }
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant_operands[0].reg_num);
        if(operandi!= 'E' && aib->dest_op_options==NULL){
            strcpy(ast->ast_error_msg,"instruction: %s expects no operands ','", aib->inst_name);
            return;
        }
        if(operandi=='E'){
            strcpy(ast->ast_error_msg,"instruction: %s expects 1 operand", aib->inst_name);
            return;
        }
        if(strchr(aib->dest_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"destination operand: %s is not supported", operandString);
            return;
        }
    }
}




project_ast lexer_get_ast(char *line){
    project_ast ret = {0};
    enum lexer_is_valid liv;
    struct asm_inst_dict *aib;
    struct asm_dir *adb;
    char *tok1,tok2;
    if(!is_trie_init) lexer_trie_init();
    SKIP_SPACES(line);
    tok1 = strchr(line,":");
    if(tok1){
        tok2 = strchr(tok1+1,':');
        if(tok2){
            strcpy(ret.ast_error_msg,"token ':' appearce twich in this line");
            ret.project_ast_options= project_ast_syntax_error;
            return ret;
        }
        switch (liv = lexer_is_valid(line)){
        case startWithoutAlpha:
            sprintf(ret.ast_error_msg,"label %s starts without an alphabetic character", line);
        break;
        case containsNoAlpha:
            sprintf(ret.ast_error_msg,"label %s contains non alphanumeric characters", line);
        break;
        case tooLong:
            sprintf(ret.ast_error_msg,"label %s is too long", line);
        break;
        case ok:
            strcpy(ret.label_name,line);
        break;
        }
        if (liv != ok){
            ret.project_ast_options = project_ast_syntax_error;
            return ret;
        }
    }
    line = tok1+1;
    SKIP_SPACES(line);
    if(*line=='\0' && ret.label_name[0]!='\0') {
        sprintf(ret.ast_error_msg,"line only contains label(%s)",ret.label_name);
        return ret;
    }
    tok1 = strpbrk(line, SPACE_CHARS);
    if(tok1) *tok1 = '\0';
    if(*line=='.') adb= trie_exists(dir_lookup,line+1);
    if(!adb) aib= trie_exists(inst_lookup,line);
    if(!aib){
        sprintf(ret.ast_error_msg,"Unkown keyword: %s",line);
        ret.project_ast_options = project_ast_syntax_error;
        return ret;
    }
    line = tok1+1;
    SKIP_SPACES(line);
    if (aib){
        ret.project_ast_options = project_ast_option;
        ret.directive_or_inst.project_ast_instant.project_ast_inst_opt = aib -> key;
        lexerParseInstructionOperands(&ret,line,aib);
    }
    if(adb){
        ret.project_ast_options = project_ast_directive;
        ret.directive_or_inst.project_ast_dir.project_ast_dir_options = adb -> key;
    }
}