#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "global.h"


/* Expressions */



enum {
    AST_TYPE_SIMPL = 1<<31,
    AST_TYPE_UNI   = 1<<30,
    AST_TYPE_BIN   = 1<<29,
    AST_TYPE_STM   = 1<<28
} ast_type_t;

typedef enum {
    EXPR_NONE       = 0,

    /* Basic types */
    EXPR_BOOL       = 1<<0 | AST_TYPE_SIMPL,
    EXPR_BOOL_MAYBE = 1<<1 | AST_TYPE_SIMPL, // Special case, each eval() needs a rand()
    EXPR_INT        = 1<<2 | AST_TYPE_SIMPL,
    EXPR_STR        = 1<<3 | AST_TYPE_SIMPL,
    EXPR_LIST       = 1<<4 | AST_TYPE_SIMPL,
    EXPR_FCALL      = 1<<5 | AST_TYPE_SIMPL,
    EXPR_VAR        = 1<<6 | AST_TYPE_SIMPL,

    /* Unary operators */
    EXPR_NOT        = 1<<0 | AST_TYPE_UNI,

    /* Binary operators */
    EXPR_EQ         = 1<<0 | AST_TYPE_BIN,
    EXPR_NE         = 1<<1 | AST_TYPE_BIN,
    EXPR_LT         = 1<<2 | AST_TYPE_BIN,
    EXPR_LE         = 1<<3 | AST_TYPE_BIN,
    EXPR_GT         = 1<<4 | AST_TYPE_BIN,
    EXPR_GE         = 1<<5 | AST_TYPE_BIN,
    EXPR_OR         = 1<<6 | AST_TYPE_BIN,
    EXPR_AND        = 1<<7 | AST_TYPE_BIN,
    EXPR_ADD        = 1<<8 | AST_TYPE_BIN,
    EXPR_SUB        = 1<<9 | AST_TYPE_BIN,
    EXPR_MUL        = 1<<10 | AST_TYPE_BIN,
    EXPR_DIV        = 1<<11 | AST_TYPE_BIN,

    STM_NOP         = 1<<1 | AST_TYPE_STM,
    STM_ASSIG       = 1<<2 | AST_TYPE_STM,
    STM_IF          = 1<<3 | AST_TYPE_STM,
    STM_WHILE       = 1<<4 | AST_TYPE_STM,
    STM_SAY         = 1<<5 | AST_TYPE_STM,
    STM_DEBUG       = 1<<6 | AST_TYPE_STM
} ast_code_t;

typedef union {
    bool  _bool;
    int   _int;
    char *_str;
    void *_list;         /* TODO */
} ast_value_t;

typedef struct ast_ts {
    ast_code_t     code;
    struct ast_ts *next;
} ast_t;

typedef struct {
    ast_t       parent;
    ast_value_t val;
} expression_value_t;

typedef struct {
    ast_t               parent;
    char               *name;
    expression_value_t  value;
} expression_var_t;

typedef struct {
    ast_t  parent;
    ast_t *argv;
    int    argc;
    ast_t *body;
} expression_fcall_t;

typedef struct {
    ast_t  parent;
    ast_t *ast1;
} expression_uni_t;

typedef struct {
    ast_t  parent;
    ast_t *ast1;
    ast_t *ast2;
} expression_bin_t;



/* Statements */

typedef struct {
    ast_t  parent;
    ast_t *lval;
    ast_t *rval;
} statement_assig_t;

typedef struct {
    ast_t  parent;
    ast_t *condition;
    ast_t *statement_then;
    ast_t *statement_else;
} statement_if_t;

typedef struct {
    ast_t  parent;
    ast_t *condition;
    ast_t *statement_while;
} statement_while_t;

typedef struct {
    ast_t  parent;
    char  *data;
} statement_print_t;



expression_value_t eval(ast_t *expr);

ast_t *new_value(ast_value_t value, ast_code_t code);
ast_t *new_cons_uni(ast_t *expr1, ast_code_t code);
ast_t *new_cons_bin(ast_t *expr1, ast_t *expr2, ast_code_t code);
ast_t *new_fcall(char *name, ast_t *argv, int argc, ast_t *body);

ast_t *new_assig(ast_t *lval, ast_t *rval);



#endif /* INTERPRETER_H */
