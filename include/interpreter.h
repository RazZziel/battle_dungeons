#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "global.h"


/* Expressions */

enum {
    EXPR_TYPE_SIMPL = 1<<31,
    EXPR_TYPE_UNI   = 1<<30,
    EXPR_TYPE_BIN   = 1<<29
};

typedef enum {
    /* Basic types */
    EXPR_BOOL       = 1<<0 | EXPR_TYPE_SIMPL,
    EXPR_BOOL_MAYBE = 1<<1 | EXPR_TYPE_SIMPL, // Special case, each eval() needs a rand()
    EXPR_INT        = 1<<2 | EXPR_TYPE_SIMPL,
    EXPR_STR        = 1<<3 | EXPR_TYPE_SIMPL,
    EXPR_LIST       = 1<<4 | EXPR_TYPE_SIMPL,
    EXPR_FCALL      = 1<<5 | EXPR_TYPE_SIMPL,
    EXPR_VAR        = 1<<6 | EXPR_TYPE_SIMPL,

    /* Unary operators */
    EXPR_NOT        = 1<<0 | EXPR_TYPE_UNI,

    /* Binary operators */
    EXPR_EQ         = 1<<0 | EXPR_TYPE_BIN,
    EXPR_NE         = 1<<1 | EXPR_TYPE_BIN,
    EXPR_LT         = 1<<2 | EXPR_TYPE_BIN,
    EXPR_LE         = 1<<3 | EXPR_TYPE_BIN,
    EXPR_GT         = 1<<4 | EXPR_TYPE_BIN,
    EXPR_GE         = 1<<5 | EXPR_TYPE_BIN,
    EXPR_OR         = 1<<6 | EXPR_TYPE_BIN,
    EXPR_AND        = 1<<7 | EXPR_TYPE_BIN,
    EXPR_ADD        = 1<<8 | EXPR_TYPE_BIN,
    EXPR_SUB        = 1<<9 | EXPR_TYPE_BIN,
    EXPR_MUL        = 1<<10 | EXPR_TYPE_BIN,
    EXPR_DIV        = 1<<11 | EXPR_TYPE_BIN
} expression_code_t;

typedef union {
    bool  _bool;
    int   _int;
    char *_str;
    void *_list;                /* TODO */
} expression_type_t;

typedef struct {
    expression_code_t code;     // Expression type
} expression_t;

typedef struct {
    expression_t      expr;
    expression_type_t val;
} expression_simpl_t;

typedef struct {
    expression_t expr;
    //fcall_t      fcall;
} expression_fcall_t;

typedef struct {
    expression_t        expr;
    char               *name;
    expression_simpl_t  value;
} expression_var_t;

typedef struct {
    expression_t  expr;
    expression_t *expr1;
} expression_uni_t;

typedef struct {
    expression_t  expr;
    expression_t *expr1;
    expression_t *expr2;
} expression_bin_t;



/* Statements */

typedef enum {
    STM_NOP,
    STM_ASSIG,
    STM_IF,
    STM_WHILE,
    STM_SAY,
    STM_DEBUG
} statement_code_t;

typedef struct statement_tm {
    statement_code_t     code;
    struct statement_tm *next;
} statement_t;

typedef struct {
    statement_t   statement;
    expression_t *lval;
    expression_t *rval;
} statement_assig_t;

typedef struct {
    statement_t   statement;
    expression_t *condition;
    statement_t  *statement_then;
    statement_t  *statement_else;
} statement_if_t;

typedef struct {
    statement_t   statement;
    expression_t *condition;
    statement_t  *statement_while;
} statement_while_t;

typedef struct {
    statement_t  statement;
    char        *data;
} statement_print_t;


typedef struct {
    statement_t   function;
    expression_t *argv;
    int           argc;
} fcall_t;



expression_simpl_t eval(expression_t *expr);
void execute(statement_t *stm);

expression_t *new_expr(expression_t *expr, expression_code_t code);
expression_t *new_expr_simpl(expression_type_t value, expression_code_t code);
expression_t *new_expr_fcall(fcall_t value, expression_code_t code);
expression_t *new_expr_uni(expression_t *expr1, expression_code_t code);
expression_t *new_expr_bin(expression_t *expr1, expression_t *expr2, expression_code_t code);

statement_t *new_stm(statement_t *stm, statement_code_t code);
statement_t *new_stm_assig(expression_t *lval, expression_t *rval);



#endif /* INTERPRETER_H */
