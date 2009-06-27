#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "interpreter.h"

void init_symbol_tables();
void free_symbol_tables();
void *check_table_size( void *cache, int cache_lines,
                        int *cache_size, int element_size );

typedef struct {
    char *name;
    ast_t *ast;
} symbol_t;

#endif /* SYMTABLE_H */
