#include "global.h"
#include "interpreter.h"
#include "errors.h"
#include "ui.h"


ast_t *init_parent(ast_t *expr, ast_code_t code);
expression_value_t eval_value(ast_t *ast);
expression_value_t eval_uni(ast_t *ast);
expression_value_t eval_bin(ast_t *exp);
expression_value_t eval_stm(ast_t *ast);


extern int yyerror(const char *fmt, ...);
expression_value_t ast_null = { {EXPR_NONE}, {0} };


char *code2str(ast_code_t code)
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
    default: return NULL;
    }
}



/*
 * AST construction
 */


ast_t *init_parent(ast_t *ast,
                   ast_code_t code)
{
    printf("______%p\n",ast);
    print_trace();
    printf("%s\n",code2str(code));
    if ( code2str(code) == NULL )
    {
        die( "Invalid AST code" );
    }

    ast->code = code;
    return ast;
}

ast_t *new_value(ast_value_t value,
                 ast_code_t code)
{
    expression_value_t *ast = malloc(sizeof(*ast));
    init_parent( (ast_t*) ast, code );
    ast->val = value;
    return (ast_t*) ast;
}

ast_t *new_fcall(char *name,
                 ast_t *argv, int argc,
                 ast_t *body)
{
    expression_fcall_t *ast = malloc(sizeof(*ast));
    init_parent( (ast_t*) ast, EXPR_FCALL );
    ast->argv = argv;
    ast->argc = argc;
    ast->body = body;
    return (ast_t*) ast;
}

ast_t *new_cons_uni(ast_t *ast1,
                    ast_code_t code)
{
    expression_uni_t *ast = malloc(sizeof(*ast));
    init_parent( (ast_t*) ast, code );
    ast->ast1 = ast1;
    return (ast_t*) ast;
}

ast_t *new_cons_bin(ast_t *ast1,
                    ast_t *ast2,
                    ast_code_t code)
{
    expression_bin_t *ast = malloc(sizeof(*ast));
    init_parent( (ast_t*) ast, code );
    ast->ast1 = ast1;
    ast->ast2 = ast2;
    return (ast_t*) ast;
}

ast_t *new_assig(ast_t *lval, ast_t *rval)
{
    if (lval->code != EXPR_VAR)
    {
        yyerror( "Invalid assignment, lvalue must be a variable, "
                 "got %s (0x%X) instead\n",
                 code2str(lval->code), lval->code & 0x00ffffff );
    }

    statement_assig_t *stm = malloc(sizeof(*stm));
    init_parent( (ast_t*) stm, STM_ASSIG );
    stm->lval = lval;
    stm->rval = rval;
    return (ast_t*) stm;
}



/*
 * AST evaluation
 */



expression_value_t eval(ast_t *ast)
{
    switch( ast->code | 0xf0000000 )
    {
    case AST_TYPE_SIMPL:
        /* Basic types */
        return eval_value( ast );

    case AST_TYPE_UNI:
        /* Unary operators */
        return eval_uni( ast );

    case AST_TYPE_BIN:
        /* Binary operators */
        return eval_bin( ast );

    case AST_TYPE_STM:
        /* Binary operators */
        return eval_stm( ast );

    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
        return ast_null;
    }
}

expression_value_t eval_value(ast_t *ast)
{
    expression_value_t *ast_value = (expression_value_t*) ast;
    expression_value_t  ret;

    switch( ast->code )
    {
    case EXPR_BOOL_MAYBE:
        init_parent( (ast_t*) &ret, EXPR_BOOL );
        ret.val._bool = ((rand() % 100) < ast_value->val._int);
        break;
    case EXPR_BOOL:
    case EXPR_INT:
    case EXPR_STR:
    case EXPR_LIST:
        ret.val._list = ast_value->val._list;
        break;
    case EXPR_VAR:
        /* TODO */
        init_parent( (ast_t*) &ret, EXPR_INT );
        ret.val._int = 0;
        break;
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return ret;
}

expression_value_t eval_uni(ast_t *ast)
{
    expression_uni_t   *ast_uni = (expression_uni_t*) ast;
    expression_value_t  ast1    = eval(ast_uni->ast1);
    expression_value_t  ret;

    switch( ast->code )
    {
    case EXPR_NOT:
        switch ( ast1.parent.code )
        {
        case EXPR_BOOL:
            init_parent( (ast_t*) &ret, EXPR_BOOL );
            ret.val._bool = ! ast1.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return ret;
}

expression_value_t eval_bin(ast_t *ast)
{
    expression_bin_t   *ast_bin = (expression_bin_t*) ast;
    expression_value_t  ast1     = eval(ast_bin->ast1);
    expression_value_t  ast2     = eval(ast_bin->ast2);
    expression_value_t  ret;

    switch( ast->code )
    {
    case EXPR_EQ:
        init_parent( (ast_t*) &ret, EXPR_BOOL );
        if (ast1.parent.code == ast2.parent.code)
        {
            switch ( ast1.parent.code )
            {
            case EXPR_BOOL:
                ret.val._bool = (ast1.val._bool == ast2.val._bool);
                break;
            case EXPR_INT:
                ret.val._bool = (ast1.val._int == ast2.val._int);
                break;
            case EXPR_STR:
                ret.val._bool = !strcmp(ast1.val._str, ast2.val._str);
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
        init_parent( (ast_t*) &ret, EXPR_BOOL );
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_BOOL:
            ret.val._int = ast1.val._bool || ast2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_AND:
        init_parent( (ast_t*) &ret, EXPR_BOOL );
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_BOOL:
            ret.val._int = ast1.val._bool && ast2.val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_ADD:
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) &ret, EXPR_INT );
            ret.val._int  = ast1.val._int + ast2.val._int;
            break;
        case EXPR_STR:
            init_parent( (ast_t*) &ret, EXPR_STR );
            ret.val._str  = malloc(strlen(ast1.val._str) + strlen(ast2.val._str) + 1);
            sprintf( ret.val._str, "%s%s", ast1.val._str, ast2.val._str );
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_SUB:
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) &ret, EXPR_INT );
            ret.val._int  = ast1.val._int - ast2.val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_MUL:
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) &ret, EXPR_INT );
            ret.val._int = ast1.val._int * ast2.val._int;
            break;
        case EXPR_STR | EXPR_INT:
        {
            char *str1  = ( ast1.parent.code == EXPR_STR
                            ? ast1.val._str
                            : ast2.val._str );
            int   times = ( ast1.parent.code == EXPR_INT
                            ? ast1.val._int
                            : ast1.val._int );
            int i;
            init_parent( (ast_t*) &ret, EXPR_STR );
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
        switch (ast1.parent.code | ast2.parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) &ret, EXPR_INT );
            ret.val._int = ast1.val._int / ast2.val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return ret;
}


expression_value_t eval_stm(ast_t *ast)
{
    expression_value_t ret = ast_null;
    
    switch( ast->code )
    {
    case STM_NOP:
        break;

    case STM_IF:
        {
            statement_if_t *ast_if = (statement_if_t*) ast;
            ret = eval_stm( eval(ast_if->condition).val._bool
                            ? ast_if->statement_then
                            : ast_if->statement_else );
        }
        break;

    case STM_WHILE:
        {
            statement_while_t *ast_while = (statement_while_t*) ast;
            while ( eval(ast_while->condition).val._bool )
                ret = eval_stm( ast_while->statement_while );
        }
        break;

    case STM_SAY:
        {
            statement_print_t *ast_print = (statement_print_t*) ast;
            message( ast_print->data );
        }
        break;

    case STM_DEBUG:
        {
            statement_print_t *ast_print = (statement_print_t*) ast;
            die( ast_print->data );
        }
        break;

    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    if ( ast->next )
        return eval_stm( ast->next );
    else
        return ret;
    /* Remember to use (-foptimize-sibling-calls|-O2|-O3|-Os)
       for tail-recursion optimization */
}
