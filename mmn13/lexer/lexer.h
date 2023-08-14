#ifndef __lexer_H_
#define __lexer_H_
#define MAX_NUMBER_DATA 80
#define MAX_LABEL_LENGTH 31

struct project_ast {
    char ast_error_msg[150];
    char label_name[MAX_LABEL_LENGTH+1];
    enum {
        project_ast_option,
        project_ast_directive
    } project_ast_options;
    union{
        struct{
            enum{
            project_ast_directive_external,
            project_ast_directive_entry,
            project_ast_directive_string,
            project_ast_directive_data
            } project_ast_dir_options;
            union{
                char * labelName;
                char * string;
                struct {
                    int data[MAX_NUMBER_DATA];
                    int dataCount;
                } data;
            } dir_operands;
        } project_ast_dir;
        struct{
            enum{
                project_ast_inst_mov,
                project_ast_inst_cmp,
                project_ast_inst_add,
                project_ast_inst_sub,
                project_ast_inst_lea,

                project_ast_inst_not,
                project_ast_inst_clr,
                project_ast_inst_inc,
                project_ast_inst_dec,
                project_ast_inst_jmp,
                project_ast_inst_bne,
                project_ast_inst_red,
                project_ast_inst_prn,
                project_ast_inst_jsr,

                project_ast_inst_rts,
                project_ast_inst_stop
            } project_ast_inst_opt;
            enum{
                project_ast_operand_option_none =0,
                project_ast_operand_option_const_number =1,
                project_ast_operand_option_label =3,
                project_ast_operand_option_register_number =5
            }project_ast_inst_operand_options[2];
            union{
                int const_num;
                int reg_num;
                char * label;
            }project_ast_instant_operands[2];
        }project_ast_instant;
    } directive_or_inst;
    };

typedef struct project_ast project_ast;


void lexer_delcat_mem();

project_ast lexer_get_ast(char *logical_line);




#endif


