#include "global.h"
#include "interpreter.h"
#include "errors.h"
#include "parser.h"
#include "symtable.h"
#include "ui.h"


ast_t *init_parent(ast_t *expr, ast_code_t code);
expression_value_t *eval_expr(ast_t *ast);
expression_value_t *eval_value(ast_t *ast);
expression_value_t *eval_uni(ast_t *ast);
expression_value_t *eval_bin(ast_t *exp);
expression_value_t *eval_stm(ast_t *ast);


extern int yyerror(const char *fmt, ...);
extern game_parser_t parser;
expression_value_t ast_null = { {EXPR_NONE}, {0} };


char *code2str(ast_code_t code)
{
    switch( code )
    {
    //case AST_TYPE_SIMPL: return "AST_TYPE_SIMPL";
    //case AST_TYPE_UNI: return "AST_TYPE_UNI";
    //case AST_TYPE_BIN: return "AST_TYPE_BIN";
    //case AST_TYPE_STM: return "AST_TYPE_STM";

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

    case STM_NOP: return "STM_NOP";
    case STM_ASSIG: return "STM_ASSIG";
    case STM_IF: return "STM_IF";
    case STM_WHILE: return "STM_WHILE";
    case STM_SAY: return "STM_SAY";
    case STM_DEBUG: return "STM_DEBUG";
    default: return NULL;
    }
}

/* Simbol table */
ast_t* lookup_symbol(char *name)
{
    debug("(lookup) %s", name);
    for (int i=0; i<parser.symbol_table_lines; i++)/*TODO:{b,h}search*/
    {
        if ( !strcmp(name, parser.symbol_table[i].name) )
        {
            return parser.symbol_table[i].ast;
        }
    }
    return NULL;
}

void add_symbol(char *name, ast_t *ast)
{
    /* TODO:
       - this is repeated multiple times in parser.y, refactor in symtable.c
       - optimice assigning each variable a unique incremental number at
         compile time so that symbol lookups are O(1)
    */

    /*TODO: copypaste sucks*/
    for (int i=0; i<parser.symbol_table_lines; i++)/*TODO:{b,h}search*/
    {
        if ( !strcmp(name, parser.symbol_table[i].name) )
        {
            debug("(update) %s", name);
            free(parser.symbol_table[i].ast);
            parser.symbol_table[i].ast = ast;
            return;
        }
    }

    debug("(add) %s", name);
    parser.symbol_table = check_table_size( parser.symbol_table,
                                            parser.symbol_table_lines,
                                            &parser.symbol_table_size,
                                            sizeof(*parser.symbol_table) );
    symbol_t symbol = { name, ast };
    parser.symbol_table[parser.symbol_table_lines++] = symbol;
}


/*
 * AST construction
 */


ast_t *init_parent(ast_t *ast,
                   ast_code_t code)
{
    //printf("(%s)\n",code2str(code));
    if ( code2str(code) == NULL )
    {
        die( "Invalid AST code\n" );
    }

    ast->code = code;
    return ast;
}

ast_t *new_value(ast_value_t value,
                 ast_code_t code)
{
    expression_value_t *ast = malloc(sizeof(*ast));
    memset( ast, 0, sizeof(*ast) );
    init_parent( (ast_t*) ast, code );
    ast->val = value;
    return (ast_t*) ast;
}

ast_t *new_fcall(char *name, ast_t *argv)
{
    expression_fcall_t *ast = malloc(sizeof(*ast));
    memset( ast, 0, sizeof(*ast) );
    init_parent( (ast_t*) ast, EXPR_FCALL );
    ast->name = name;
    ast->argv = argv;
    ast->argc = 0;/*TODO*/
    return (ast_t*) ast;
}

ast_t *new_cons_uni(ast_t *ast1,
                    ast_code_t code)
{
    expression_uni_t *ast = malloc(sizeof(*ast));
    memset( ast, 0, sizeof(*ast) );
    init_parent( (ast_t*) ast, code );
    ast->ast1 = ast1;
    return (ast_t*) ast;
}

ast_t *new_cons_bin(ast_t *ast1,
                    ast_t *ast2,
                    ast_code_t code)
{
    expression_bin_t *ast = malloc(sizeof(*ast));
    memset( ast, 0, sizeof(*ast) );
    init_parent( (ast_t*) ast, code );
    ast->ast1 = ast1;
    ast->ast2 = ast2;
    return (ast_t*) ast;
}

ast_t *new_assig(ast_t *lval, ast_t *rval)
{
    if (lval->code != EXPR_VAR)
    {
        die( "Invalid assignment, lvalue must be a variable, "
             "got %s (0x%X) instead\n",
             code2str(lval->code), lval->code & 0x00ffffff );
    }

    //debug( ">%p %s\n", rval, code2str(rval->code) );

    statement_assig_t *ast = malloc(sizeof(*ast));
    memset( ast, 0, sizeof(*ast) );
    init_parent( (ast_t*) ast, STM_ASSIG );
    ast->lval = lval;
    ast->rval = rval;
    return (ast_t*) ast;
}



/*
 * AST evaluation
 */

void print_expr(char *name, expression_value_t *ast)
{
    switch ( ast->parent.code )
    {
    case EXPR_BOOL:
        debug( "%s=%s", name, ast->val._bool ? "true" : "false" );
        break;
    case EXPR_INT:
        debug( "%s=%d", name, ast->val._int );
        break;
    case EXPR_STR:
        debug( "%s=%s", name, ast->val._str );
        break;
    default:
        debug( "%s=%x", name, ast->val._int );
    }
    //debug( "-- (%s,%d)", code2str(((ast_t*)ast)->code), ast->next ? 1 : 0 );    
}

expression_value_t *eval(ast_t *ast)
{
    expression_value_t *ret =  eval_expr( ast );

    //print_expr( ret );

    if ( ast->next )
        return eval( ast->next );
    else
        return ret;
    /* Remember to use (-foptimize-sibling-calls|-O2|-O3|-Os)
       for tail-recursion optimization */
}

expression_value_t *eval_expr(ast_t *ast)
{
    switch( ast->code & 0xff000000 )
    {
    case AST_TYPE_SIMPL:
        /* Basic types */
        return eval_value( ast );
        break;

    case AST_TYPE_UNI:
        /* Unary operators */
        return eval_uni( ast );
        break;

    case AST_TYPE_BIN:
        /* Binary operators */
        return eval_bin( ast );
        break;

    case AST_TYPE_STM:
        /* Binary operators */
        return eval_stm( ast );
        break;

    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return &ast_null;
}

expression_value_t *eval_value(ast_t *ast)
{
    expression_value_t *ast_value = (expression_value_t*) ast;
    expression_value_t *ret = malloc(sizeof(*ret));
    memset( ret, 0, sizeof(*ret) );

    switch( ast->code )
    {
    case EXPR_BOOL_MAYBE:
        init_parent( (ast_t*) ret, EXPR_BOOL );
        ret->val._bool = ((rand() % 100) < ast_value->val._int);
        break;
    case EXPR_BOOL:
    case EXPR_INT:
    case EXPR_STR:
    case EXPR_LIST:
        init_parent( (ast_t*) ret, ast->code );
        ret->val = ast_value->val;
        break;
    case EXPR_VAR:
    {
        /* TODO */
        expression_var_t *ast_var   = (expression_var_t*) ast;
        ast_t            *ast_value = lookup_symbol( ast_var->name );
        if (ast_value == NULL)
        {
            die( "Symbol %s not defined", ast_var->name );
        }
        expression_value_t *value = (expression_value_t*) ast_value;
        init_parent( (ast_t*) ret, value->parent.code );
        ret->val = value->val;
        print_expr( ast_var->name, value );
        break;
    }
    case EXPR_FCALL:
        /*TODO: lookup function in symbol table, evaluate and return*/
        /*
          expression_fcall_t *ast_fcall = (expression_fcall_t*) ast;
          expression_value_t *value     = eval( function_lookup( ast_var->name ) );
          init_parent( (ast_t*) ret, value->parent.code );
          ret->val = value->val;
        */
        break;
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return ret;
}

expression_value_t *eval_uni(ast_t *ast)
{
    expression_uni_t   *ast_uni = (expression_uni_t*) ast;
    expression_value_t *ast1    = eval(ast_uni->ast1);
    expression_value_t *ret = malloc(sizeof(*ret));
    memset( ret, 0, sizeof(*ret) );

    switch( ast->code )
    {
    case EXPR_NOT:
        switch ( ast1->parent.code )
        {
        case EXPR_BOOL:
            init_parent( (ast_t*) ret, EXPR_BOOL );
            ret->val._bool = ! ast1->val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    free( ast1 );

    return ret;
}

expression_value_t *eval_bin(ast_t *ast)
{
    expression_bin_t   *ast_bin = (expression_bin_t*) ast;
    expression_value_t *ast1    = eval(ast_bin->ast1);
    expression_value_t *ast2    = eval(ast_bin->ast2);
    expression_value_t *ret = malloc(sizeof(*ret));
    memset( ret, 0, sizeof(*ret) );

    switch( ast->code )
    {
    case EXPR_EQ:
        init_parent( (ast_t*) ret, EXPR_BOOL );
        if (ast1->parent.code == ast2->parent.code)
        {
            switch ( ast1->parent.code )
            {
            case EXPR_BOOL:
                ret->val._bool = (ast1->val._bool == ast2->val._bool);
                break;
            case EXPR_INT:
                ret->val._bool = (ast1->val._int == ast2->val._int);
                break;
            case EXPR_STR:
                ret->val._bool = !strcmp(ast1->val._str, ast2->val._str);
                break;
            case EXPR_LIST:
                ret->val._bool = 0; /* TODO */
                break;
            case EXPR_VAR:
                ret->val._bool = 0; /* TODO */
                break;
            default:
                die( "bleh\n" );
            }
        }
        else
        {
            ret->val._bool = 0;
        }
        break;

    case EXPR_OR:
        init_parent( (ast_t*) ret, EXPR_BOOL );
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_BOOL:
            ret->val._int = ast1->val._bool || ast2->val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_AND:
        init_parent( (ast_t*) ret, EXPR_BOOL );
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_BOOL:
            ret->val._int = ast1->val._bool && ast2->val._bool;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_ADD:
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) ret, EXPR_INT );
            ret->val._int = ast1->val._int + ast2->val._int;
            break;
        case EXPR_STR:
            init_parent( (ast_t*) ret, EXPR_STR );
            ret->val._str = malloc(strlen(ast1->val._str) + strlen(ast2->val._str) + 1);
            sprintf( ret->val._str, "%s%s", ast1->val._str, ast2->val._str );
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_SUB:
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) ret, EXPR_INT );
            ret->val._int  = ast1->val._int - ast2->val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_MUL:
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) ret, EXPR_INT );
            ret->val._int = ast1->val._int * ast2->val._int;
            break;
        case EXPR_STR | EXPR_INT:
        {
            char *str1  = ( ast1->parent.code == EXPR_STR
                            ? ast1->val._str
                            : ast2->val._str );
            int   times = ( ast1->parent.code == EXPR_INT
                            ? ast1->val._int
                            : ast2->val._int );
            int i;
            init_parent( (ast_t*) ret, EXPR_STR );
            ret->val._str = malloc((strlen(str1) * times) + 1);
            for (i=0; i<times; i++)
            {
                /*TODO: Optimize with memcpy()+offset*/
                strcat( ret->val._str, str1 );
            }
            break;
        }
        default:
            die( "incompatible types\n" );
        }
        break;

    case EXPR_DIV:
        switch (ast1->parent.code | ast2->parent.code)
        {
        case EXPR_INT:
            init_parent( (ast_t*) ret, EXPR_INT );
            ret->val._int = ast1->val._int / ast2->val._int;
            break;
        default:
            die( "incompatible types\n" );
        }
        break;
    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    free( ast1 );
    free( ast2 );

    return ret;
}


expression_value_t *eval_stm(ast_t *ast)
{
    expression_value_t *ret = &ast_null;
    switch( ast->code )
    {
    case STM_NOP:
        break;

    case STM_ASSIG:
    {
        statement_assig_t *ast_assig = (statement_assig_t*) ast;
        expression_var_t  *ast_var   = (expression_var_t*) ast_assig->lval;
        ast_t             *ast_value = (ast_t*) eval(ast_assig->rval);

        //debug( "<%p %s\n", ast_assig->rval, code2str(ast_assig->rval->code) );

        add_symbol( ast_var->name, ast_value );
        break;
    }

    case STM_IF:
    {
        statement_if_t *ast_if = (statement_if_t*) ast;
        expression_value_t *condition = eval(ast_if->condition);

        if (condition->parent.code != EXPR_BOOL)
            die( "Condition must be %s, got %s instead\n",
                 code2str(EXPR_BOOL),
                 code2str(condition->parent.code) );
        
        ret = eval_stm( condition->val._bool
                        ? ast_if->statement_then
                        : ast_if->statement_else );
    }
    break;

    case STM_WHILE:
    {
        statement_while_t *ast_while = (statement_while_t*) ast;
        while ( eval(ast_while->condition)->val._bool )
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
        debug( "%s", ast_print->data );
    }
    break;

    case EXPR_FCALL:
    {
        /* TODO */
    }
    break;

    default:
        die( "Invalid AST code %s\n", code2str(ast->code) );
    }

    return ret;
}

ast_t *append_to_expr( ast_t *list, ast_t *expr )
{
    ast_t *iter = list;
    while ( iter->next )
    {
        iter = iter->next;
    }
    iter->next = expr;
    return list;
}
