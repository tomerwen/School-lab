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
    {"mov", project_ast_inst_mov, "MYR", "YR"},
    {"cmp", project_ast_inst_cmp, "MYR", "MYR"},
    {"add", project_ast_inst_add, "MYR", "YR"},
    {"sub", project_ast_inst_sub, "MYR", "YR"},
    {"lea", project_ast_inst_lea, "Y", "YR"},
    {"not", project_ast_inst_not, NULL, "YR"},
    {"clr", project_ast_inst_clr, NULL, "YR"},
    {"inc", project_ast_inst_inc, NULL, "YR"},
    {"dec", project_ast_inst_dec, NULL, "YR"},
    {"jmp", project_ast_inst_jmp, NULL, "YR"},
    {"bne", project_ast_inst_bne, NULL, "YR"},
    {"red", project_ast_inst_red, NULL, "YR"},
    {"prn", project_ast_inst_prn, NULL, "MYR"},
    {"jsr", project_ast_inst_jsr, NULL, "YR"},
    {"rts", project_ast_inst_rts, NULL, NULL},
    {"stop", project_ast_inst_stop, NULL, NULL}
};

static struct asm_dir{
    const char * name;
    int key;
} asm_dir[4] = {
    {"data",project_ast_directive_data},
    {"entry",project_ast_directive_entry},
    {"extern",project_ast_directive_external},
    {"string",project_ast_directive_string}
};


static void lexer_trie_init(){
    int i;
    inst_lookup = trie();
    dir_lookup = trie();
    for(i=0;i<16;i++) {
        trie_insert(inst_lookup,asm_inst_dict[i].inst_name,&asm_inst_dict[i]);
    }
    for(i=0;i<4;i++) {
        trie_insert(dir_lookup,asm_dir[i].name,&asm_dir[i]);
    }
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
    char * my_end;
    *num = strtol(num_str,&my_end,10);
    errno =0;
    while(isspace(*my_end)) my_end++;
    if(*my_end !='\0') return -1;
    if(errno==ERANGE) return -2;
    if(*num>max || *num < min) return -3;
    if(endptr) *endptr=my_end;
    return 0;
}
static char lexerParseOperandI(char * operandString, char ** label, int * constNumber, int * regNumber){ /* returns I/L/R or returns N=unkown, E=empty, F=num overflow*/
    char *temp = NULL;
    char * temp2 = NULL;
    long num;
    int ret;
    if(operandString==NULL) return 'E';
    SKIP_SPACES(operandString);
    if(*operandString=='\0') return 'E';
    if(*operandString == '@') {
        if((*operandString+1)=='r'){
            if(*(operandString+2) == '+' || *(operandString+2) == '-') return 'N';
            if(lexerParseNum(operandString+2,NULL,&num,MIN_REG,MAX_REG)!=0) return 'N';
            if(regNumber) *regNumber = (int)num;
            return 'R';
        }
        return 'N';
    }
    if(isalpha(*operandString)){
        temp2 = strpbrk(operandString,SPACE_CHARS);
        if(temp){
            *temp = '\0';
            temp++;
            SKIP_SPACES(temp);
            if(*temp !='\0') {
                *temp2 = ' ';
                return 'N';
            }
        } 
        if(lexer_is_valid(operandString)!=ok) return 'N';
        if(label) (*label)  = operandString;
        return 'L';
    }
    if ((ret = lexerParseNum(operandString,NULL,&num,MIN_CONST_NUM,MAX_CONST_NUM)) < -2) {
        return 'F';
    } else if (ret==0){
        if(constNumber) (*constNumber) = num;
        return 'I';
    }
    return 'N';
}


static void lexerParseInstructionOperands(project_ast * ast, char * operandString,struct  asm_inst_dict *aib){
    char operandi;
    char *tok1;
    char *seperator = NULL;
    if(operandString) seperator = strchr(operandString,',');
    else{
        if(aib->dest_op_options != NULL){
            sprintf(ast->ast_error_msg,"instruction: %s expects 1 operand", aib->inst_name);
            return;
        }
        return;
    }
    if(seperator){
        tok1 = strchr(seperator+1,',');
        if(tok1) strcpy(ast->ast_error_msg,"found at least two seperator tokens");
        if(aib->source_op_options==NULL) {
            sprintf(ast->ast_error_msg,"instruction: %s expects only 1 operand but recieved more", aib->inst_name);
            return;
        }
        *seperator = '\0';
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].reg_num);
        if(operandi == 'N'){
            sprintf(ast->ast_error_msg,"Unkown keyword: %s",operandString);
            return;
        }
        if(operandi == 'F'){
            sprintf(ast->ast_error_msg,"overflow immediate operand: %s",operandString);
            return;
        }
        if(operandi == 'E'){
            sprintf(ast->ast_error_msg,"Expected text operand for source");
            return;
        }
        if(strchr(aib->source_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"source operand: %s is not supported", operandString);
            return;
        }
        ast->directive_or_inst.project_ast_instant.project_ast_inst_operand_options[0] = operandi == 'I' ? project_ast_operand_option_const_number : operandi == 'R' ?  project_ast_operand_option_register_number : project_ast_operand_option_label; 
        operandString = seperator+1;
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].reg_num);
        if(operandi == 'N'){
            sprintf(ast->ast_error_msg,"Unkown operand: for destination %s",operandString);
            return;
        }
        if(operandi == 'F'){
            sprintf(ast->ast_error_msg,"overflow immediate operand: %s",operandString);
            return;
        }
        if(operandi == 'E'){
            sprintf(ast->ast_error_msg,"Expected text operand for destination");
            return;
        }
        if(strchr(aib->dest_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"destination operand: %s is not supported", operandString);
            return;
        }
        ast->directive_or_inst.project_ast_instant.project_ast_inst_operand_options[1] = operandi == 'I' ? project_ast_operand_option_const_number : operandi == 'R' ?  project_ast_operand_option_register_number : project_ast_operand_option_label; 
    }else{
        if(aib->source_op_options!=NULL){
            sprintf(ast->ast_error_msg,"instruction: %s expects seperator token ','", aib->inst_name);
            return;
        }
        operandi= lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].label,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].const_num,&ast->directive_or_inst.project_ast_instant.project_ast_instant_operands[0].reg_num);
        if(operandi!= 'E' && aib->dest_op_options==NULL){
            sprintf(ast->ast_error_msg,"instruction: %s expects no operands ','", aib->inst_name);
            return;
        }
        if(operandi=='E'){
            sprintf(ast->ast_error_msg,"instruction: %s expects 1 operand", aib->inst_name);
            return;
        }
        if(operandi == 'F'){
            sprintf(ast->ast_error_msg,"overflow immediate operand: %s for destination",operandString);
            return;
        }
        if(operandi == 'N'){
            sprintf(ast->ast_error_msg,"unkown operan %s for destination", operandString);
            return;
        }
        if(strchr(aib->dest_op_options,operandi)==NULL) {
            sprintf(ast->ast_error_msg,"destination operand: %s is not supported", operandString);
            return;
        }
        ast->directive_or_inst.project_ast_instant.project_ast_inst_operand_options[1] = operandi == 'I' ? project_ast_operand_option_const_number : operandi == 'R' ?  project_ast_operand_option_register_number : project_ast_operand_option_label; 
    }
}

static void lexer_parse_directive(project_ast *ast, char * operandString, struct asm_dir * adb){
    char * seperator1, * seperator2;
    if(adb->key <= project_ast_directive_entry) {
        if(lexerParseOperandI(operandString,&ast->directive_or_inst.project_ast_dir.dir_operands.labelName,NULL,NULL)!= 'L'){
            sprintf(ast->ast_error_msg,"directive %s with invalid operand %s" , adb->name,operandString);
            return;
        } 
    }
    if(adb->key == project_ast_directive_string){
        seperator1 = strstr(operandString,"\"\"");
        if(!seperator1){
            sprintf(ast->ast_error_msg,"directive %s has no opening \": %s",adb->name,operandString);
        }
        seperator1++;
        seperator2 = strstr(seperator1,"\"\"");
        if(!seperator2){
            sprintf(ast->ast_error_msg,"directive %s has no closing \": %s",adb->name,operandString);
        }
        *seperator2 = '\0';
        seperator2++;
        SKIP_SPACES(seperator2);
        if(*seperator2!='\0'){
            sprintf(ast->ast_error_msg,"directive %s has extra text after string : %s",adb->name,operandString);
        }
        ast->directive_or_inst.project_ast_dir.dir_operands.string = seperator1;
    }
    if(adb->key == project_ast_directive_data){
        int counter = 0, current;
        seperator1 = strchr(operandString,',');
        do{
            if(seperator1) *seperator1 ='\0';
            switch(lexerParseOperandI(operandString,NULL,&current,NULL)){
                case 'I':
                ast->directive_or_inst.project_ast_dir.dir_operands.data.data[counter] = current;
                counter++;
                ast->directive_or_inst.project_ast_dir.dir_operands.data.dataCount = counter;
                break;
                case 'F':
                    sprintf(ast->ast_error_msg,"directive %s overflowed number %s",adb->name,operandString);
                    return;
                break;
                case 'E':
                    sprintf(ast->ast_error_msg,"directive %s got no operand ",adb->name);
                break;
                default:
                    sprintf(ast->ast_error_msg,"directive %s got non number string %s",adb->name,operandString);
                break;
            }
            if(seperator1) {
                operandString = seperator1+1;
                
            }else break;
        }while(1);
    }
}

void lexer_delcat_mem(){
    is_trie_init = 0;
    trie_destroy(&dir_lookup);
    trie_destroy(&inst_lookup);
}


project_ast lexer_get_ast(char *line){
    project_ast ret = {0};
    enum valid_err liv;
    struct asm_inst_dict *aib = NULL;
    struct asm_dir *adb = NULL;
    char *tok1;
    char *tok2;
    if(!is_trie_init) lexer_trie_init();
    line[strcspn(line , "\r\n")] = 0;
    SKIP_SPACES(line);
    tok1 = strchr(line,':');
    if(tok1){
        tok2 = strchr(tok1+1,':');
        if(tok2){
            strcpy(ret.ast_error_msg,"token ':' appearce twich in this line");
            return ret;
        }
        (*tok1) = '\0';
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
        if (liv != ok) return ret;
        line = tok1+1;
        SKIP_SPACES(line);
    }
    if(*line=='\0' && ret.label_name[0]!='\0') {
        sprintf(ret.ast_error_msg,"line only contains label(%s)",ret.label_name);
        return ret;
    }
    tok1 = strpbrk(line, SPACE_CHARS);
    if(tok1) {
        *tok1 = '\0';
        tok1++;
        SKIP_SPACES(tok1);
    }
    if(*line=='.') {
        adb= trie_exists(dir_lookup,line+1);
        if(!adb) {
            sprintf(ret.ast_error_msg,"unkown directive %s ", line+1);
            return ret;
        }
        ret.project_ast_options = project_ast_directive;
        ret.directive_or_inst.project_ast_dir.project_ast_dir_options = adb -> key;
        lexer_parse_directive(&ret,tok1,adb);
        return ret;
    } 
    aib= trie_exists(inst_lookup,line);
    if(!aib){
        sprintf(ret.ast_error_msg,"Unkown keyword: %s",line);
        return ret;
    }
    ret.project_ast_options = project_ast_option;
    ret.directive_or_inst.project_ast_instant.project_ast_inst_opt = aib -> key;
    lexerParseInstructionOperands(&ret,tok1,aib);
    return ret;
}