#include "battle.h"
#include "errors.h"
#include "windows.h"

/* Expressions */

enum {
    EXPR_TYPE_SIMPL = 1<<31,
    EXPR_TYPE_UNI   = 1<<30,
    EXPR_TYPE_BIN   = 1<<29
};

typedef enum {
    /* Basic types */
    EXPR_BOOL       = 1<<0 | EXPR_TYPE_SIMPL,
    EXPR_BOOL_MAYBE = 1<<1 | EXPR_TYPE_SIMPL, // Special case, bool value depends on rand()
    EXPR_INT        = 1<<2 | EXPR_TYPE_SIMPL,
    EXPR_STR        = 1<<3 | EXPR_TYPE_SIMPL,
    EXPR_TUPLE      = 1<<4 | EXPR_TYPE_SIMPL,
    EXPR_VAR        = 1<<5 | EXPR_TYPE_SIMPL,

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

typedef struct {
    expression_code_t code;
    union {
        bool  _bool;
        int   _int;
        char *_str;
        void *_tuple; /* TODO */
    } val;
} expression_val_t;

typedef struct {
    expression_code_t code; // Expression type
} expression_t;

typedef struct {
    expression_t     expression;
    expression_val_t value;
} expression_simpl_t;

typedef struct {
    expression_t      expression;
    char             *name;
    expression_val_t  value;
} expression_var_t;

typedef struct {
    expression_t  expression;
    expression_t *expr;
} expression_uni_t;

typedef struct {
    expression_t  expression;
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





expression_val_t eval(expression_t *expr);
expression_val_t eval_simpl(expression_t *expr);
expression_val_t eval_uni(expression_t *expr);
expression_val_t eval_bin(expression_t *exp);
void execute(statement_t *stm);

expression_t *new_expr(expression_t *expr, expression_code_t code);
expression_t *new_expr_simpl(expression_val_t value,
                             expression_code_t code);
expression_t *new_expr_uni(expression_t *expr1,
                           expression_code_t code);
expression_t *new_expr_bin(expression_t *expr1,
                           expression_t *expr2,
                           expression_code_t code);

statement_t *new_stm(statement_t *stm, statement_code_t code);
statement_t *new_stm_assig(expression_t *lval, expression_t *rval);
statement_t *new_stm_decl(char *var_type, char *var_name);

expression_val_t expr_null = { 0, {0} };
expression_val_t eval(expression_t *expr)
{
    switch( expr->code | 0xf0000000 )
    {
    case EXPR_TYPE_SIMPL:
        /* Basic types */
        return eval_simpl( expr );

    case EXPR_TYPE_UNI:
        /* Unary operators */
        return eval_uni( expr );

    case EXPR_TYPE_BIN:
        /* Binary operators */
        return eval_bin( expr );

    default:
        die( "Invalid expression code %d\n", expr->code );
        return expr_null;
    }
}

expression_val_t eval_simpl(expression_t *expr)
{
    expression_simpl_t *expr_simpl = (expression_simpl_t*) expr;
    expression_val_t    ret;

    switch( expr->code )
    {
    case EXPR_BOOL_MAYBE:
        ret.code      = EXPR_BOOL;
        ret.val._bool = ((rand() % 100) < expr_simpl->value.val._int);
        break;
    case EXPR_BOOL:
    case EXPR_INT:
    case EXPR_STR:
    case EXPR_TUPLE:
        ret = expr_simpl->value;
        break;
    case EXPR_VAR:
        /* TODO */
        ret.code      = EXPR_INT;
        ret.val._int  = 0;
        break;
    default:
        die( "Invalid expression code %d\n", expr->code );
    }

    return ret;
}

expression_val_t eval_uni(expression_t *expr)
{
    expression_uni_t *expr_uni = (expression_uni_t*) expr;
    expression_val_t  expr1    = eval(expr_uni->expr);
    expression_val_t  ret;

    switch( expr->code )
    {
    case EXPR_NOT:
        switch (expr1.code)
        {
        case EXPR_BOOL:
            ret.code = EXPR_BOOL;
            ret.val._bool = ! expr1.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
    default:
        die( "Invalid expression code %d\n", expr->code );
    }

    return ret;
}

expression_val_t eval_bin(expression_t *exp)
{
    expression_bin_t *expr_bin = (expression_bin_t*) exp;
    expression_val_t  expr1    = eval(expr_bin->expr1);
    expression_val_t  expr2    = eval(expr_bin->expr2);
    expression_val_t  ret;

    switch( exp->code )
    {
    case EXPR_EQ:
        ret.code = EXPR_BOOL;
        if (expr1.code == expr2.code)
        {
            switch (expr1.code)
            {
            case EXPR_BOOL:
                ret.val._bool = (expr1.val._bool == expr2.val._bool);
                break;
            case EXPR_INT:
                ret.val._bool = (expr1.val._int == expr2.val._int);
                break;
            case EXPR_STR:
                ret.val._bool = !strcmp(expr1.val._str, expr2.val._str);
                break;
            case EXPR_TUPLE:
                ret.val._bool = 0; /* TODO */
                break;
            case EXPR_VAR:
                ret.val._bool = 0; /* TODO */
                break;
            default:
                die( "bleh\n" );
            }
        }
        else
        {
            ret.val._bool = 0;
        }
        break;

    case EXPR_OR:
        ret.code = EXPR_BOOL;
        switch (expr1.code | expr2.code)
        {
        case EXPR_BOOL:
            ret.val._int = expr1.val._bool || expr2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_AND:
        ret.code = EXPR_BOOL;
        switch (expr1.code | expr2.code)
        {
        case EXPR_BOOL:
            ret.val._int = expr1.val._bool && expr2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_ADD:
        switch (expr1.code | expr2.code)
        {
        case EXPR_INT:
            ret.code     = EXPR_INT;
            ret.val._int = expr1.val._int + expr2.val._int;
            break;
        case EXPR_STR:
            ret.code     = EXPR_STR;
            ret.val._str = malloc(strlen(expr1.val._str) + strlen(expr2.val._str) + 1);
            sprintf( ret.val._str, "%s%s", expr1.val._str, expr2.val._str );
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_SUB:
        switch (expr1.code | expr2.code)
        {
        case EXPR_INT:
            ret.code     = EXPR_INT;
            ret.val._int = expr1.val._int - expr2.val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_MUL:
        switch (expr1.code | expr2.code)
        {
        case EXPR_INT:
            ret.code = EXPR_INT;
            ret.val._int = expr1.val._int * expr2.val._int;
            break;
        case EXPR_STR | EXPR_INT:
        {
            char *str1  = ( expr1.code == EXPR_STR
                            ? expr1.val._str
                            : expr2.val._str );
            int   times = ( expr1.code == EXPR_INT
                            ? expr1.val._int
                            : expr1.val._int );
            int i;
            ret.code = EXPR_STR;
            ret.val._str = malloc((strlen(str1) * times) + 1);
            for (i=0; i<times; i++)
            {
                /*TODO: Optimize with memcpy()+offset*/
                strcat( ret.val._str, str1 );
            }
            break;
        }
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_DIV:
        switch (expr1.code | expr2.code)
        {
        case EXPR_INT:
            ret.code     = EXPR_INT;
            ret.val._int = expr1.val._int / expr2.val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;
    default:
        die( "Invalid expression code %d\n", exp->code );
    }

    return ret;
}


void execute(statement_t *stm)
{
    if (!stm)
    {
        return;
    }
    
    switch( stm->code )
    {
    case STM_NOP:
        break;

    case STM_IF:
        {
            statement_if_t *stm_if = (statement_if_t*)stm;
            execute( eval(stm_if->condition).val._bool
                     ? stm_if->statement_then
                     : stm_if->statement_else );
        }
        break;

    case STM_WHILE:
        {
            statement_while_t *stm_while = (statement_while_t*)stm;
            while ( eval(stm_while->condition).val._bool )
                execute( stm_while->statement_while );
        }
        break;

    case STM_SAY:
        {
            statement_print_t *stm_print = (statement_print_t*)stm;
            message( stm_print->data );
        }
        break;

    case STM_DEBUG:
        {
            statement_print_t *stm_print = (statement_print_t*)stm;
            die( stm_print->data );
        }
        break;

    default:
        die( "Invalid statement code %d\n", stm->code );
    }

    execute( stm->next );
    /* Remember to use (-foptimize-sibling-calls|-O2|-O3|-Os)
       for tail-recursion optimization */
}


expression_t *new_expr(expression_t *expr, expression_code_t code)
{
    expr->code = code;
    return expr;
}

expression_t *new_expr_simpl( expression_val_t value,
                              expression_code_t code )
{
    expression_simpl_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    expr->value = value;
    return (expression_t*) expr;
}

expression_t *new_expr_uni( expression_t *expr1,
                            expression_code_t code )
{
    expression_uni_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    expr->expr = expr1;
    return (expression_t*) expr;
}

expression_t *new_expr_bin( expression_t *expr1,
                            expression_t *expr2,
                            expression_code_t code )
{
    expression_bin_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    expr->expr1 = expr1;
    expr->expr2 = expr2;
    return (expression_t*) expr;
}

statement_t *new_stm(statement_t *stm, statement_code_t code)
{
    stm->code = code;
    return stm;
}

statement_t *new_stm_assig( expression_t *lval, expression_t *rval )
{
    if (lval->code != EXPR_VAR)
    {
        die( "Invalid assignment, lvalue must be a variable" );
    }

    statement_assig_t *stm = malloc(sizeof(*stm));
    new_stm( (statement_t*) stm, STM_ASSIG );
    stm->lval = lval;
    stm->rval = rval;
    return (statement_t*) stm;
}
