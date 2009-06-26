#include "global.h"
#include "interpreter.h"
#include "errors.h"
#include "ui.h"


expression_simpl_t eval_simpl(expression_t *expr);
expression_simpl_t eval_uni(expression_t *expr);
expression_simpl_t eval_bin(expression_t *exp);


expression_simpl_t expr_null = { {0}, {0} };


char *code2str(expression_code_t code)
{
    switch( code )
    {
    case EXPR_BOOL: return "EXPR_BOOL";
    case EXPR_BOOL_MAYBE: return "EXPR_BOOL_MAYBE";
    case EXPR_INT: return "EXPR_INT";
    case EXPR_STR: return "EXPR_STR";
    case EXPR_LIST: return "EXPR_LIST";
    case EXPR_FCALL: return "EXPR_FCALL";
    case EXPR_VAR: return "EXPR_VAR";

    case EXPR_NOT: return "EXPR_NOT";

    case EXPR_EQ: return "EXPR_EQ";
    case EXPR_NE: return "EXPR_NE";
    case EXPR_LT: return "EXPR_LT";
    case EXPR_LE: return "EXPR_LE";
    case EXPR_GT: return "EXPR_GT";
    case EXPR_GE: return "EXPR_GE";
    case EXPR_OR: return "EXPR_OR";
    case EXPR_AND: return "EXPR_AND";
    case EXPR_ADD: return "EXPR_ADD";
    case EXPR_SUB: return "EXPR_SUB";
    case EXPR_MUL: return "EXPR_MUL";
    case EXPR_DIV: return "EXPR_DIV";
    default: return "UNKNOWN";
    }
}

expression_simpl_t eval(expression_t *expr)
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

expression_simpl_t eval_simpl(expression_t *expr)
{
    expression_simpl_t *expr_simpl = (expression_simpl_t*) expr;
    expression_simpl_t  ret;

    switch( expr->code )
    {
    case EXPR_BOOL_MAYBE:
        ret.expr.code = EXPR_BOOL;
        ret.val._bool = ((rand() % 100) < expr_simpl->val._int);
        break;
    case EXPR_BOOL:
    case EXPR_INT:
    case EXPR_STR:
    case EXPR_LIST:
        ret.val._list = expr_simpl->val._list;
        break;
    case EXPR_VAR:
        /* TODO */
        ret.expr.code = EXPR_INT;
        ret.val._int  = 0;
        break;
    default:
        die( "Invalid expression code %d\n", expr->code );
    }

    return ret;
}

expression_simpl_t eval_uni(expression_t *expr)
{
    expression_uni_t   *expr_uni = (expression_uni_t*) expr;
    expression_simpl_t  expr1    = eval(expr_uni->expr1);
    expression_simpl_t  ret;

    switch( expr->code )
    {
    case EXPR_NOT:
        switch ( expr1.expr.code )
        {
        case EXPR_BOOL:
            ret.expr.code = EXPR_BOOL;
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

expression_simpl_t eval_bin(expression_t *exp)
{
    expression_bin_t   *expr_bin = (expression_bin_t*) exp;
    expression_simpl_t  expr1    = eval(expr_bin->expr1);
    expression_simpl_t  expr2    = eval(expr_bin->expr2);
    expression_simpl_t  ret;

    switch( exp->code )
    {
    case EXPR_EQ:
        ret.expr.code = EXPR_BOOL;
        if (expr1.expr.code == expr2.expr.code)
        {
            switch ( expr1.expr.code )
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
            case EXPR_LIST:
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
        ret.expr.code = EXPR_BOOL;
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_BOOL:
            ret.val._int = expr1.val._bool || expr2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_AND:
        ret.expr.code = EXPR_BOOL;
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_BOOL:
            ret.val._int = expr1.val._bool && expr2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_ADD:
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_INT:
            ret.expr.code = EXPR_INT;
            ret.val._int  = expr1.val._int + expr2.val._int;
            break;
        case EXPR_STR:
            ret.expr.code = EXPR_STR;
            ret.val._str  = malloc(strlen(expr1.val._str) + strlen(expr2.val._str) + 1);
            sprintf( ret.val._str, "%s%s", expr1.val._str, expr2.val._str );
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_SUB:
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_INT:
            ret.expr.code = EXPR_INT;
            ret.val._int  = expr1.val._int - expr2.val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_MUL:
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_INT:
            ret.expr.code = EXPR_INT;
            ret.val._int = expr1.val._int * expr2.val._int;
            break;
        case EXPR_STR | EXPR_INT:
        {
            char *str1  = ( expr1.expr.code == EXPR_STR
                            ? expr1.val._str
                            : expr2.val._str );
            int   times = ( expr1.expr.code == EXPR_INT
                            ? expr1.val._int
                            : expr1.val._int );
            int i;
            ret.expr.code = EXPR_STR;
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
        switch (expr1.expr.code | expr2.expr.code)
        {
        case EXPR_INT:
            ret.expr.code = EXPR_INT;
            ret.val._int  = expr1.val._int / expr2.val._int;
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

expression_t *new_expr_simpl( expression_type_t value,
                              expression_code_t code )
{
    expression_simpl_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    expr->val = value;
    return (expression_t*) expr;
}

expression_t *new_expr_fcall(fcall_t fcall,
                             expression_code_t code)
{
    expression_fcall_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    //expr->fcall = fcall;
    return (expression_t*) expr;
}

expression_t *new_expr_uni( expression_t *expr1,
                            expression_code_t code )
{
    expression_uni_t *expr = malloc(sizeof(*expr));
    new_expr( (expression_t*) expr, code );
    expr->expr1 = expr1;
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
        die( "Invalid assignment, lvalue must be a variable, got %s (0x%X) instead\n",
             code2str(lval->code), lval->code & 0x00ffffff );
    }

    statement_assig_t *stm = malloc(sizeof(*stm));
    new_stm( (statement_t*) stm, STM_ASSIG );
    stm->lval = lval;
    stm->rval = rval;
    return (statement_t*) stm;
}
