#include <string.h>
#include "battle.h"

void load_bd (FILE *file, char *filename, char *item) {
#define INDENT 60
  printf( " * Cargando base de datos de %s...  ", item );
  if (file != NULL) {
      printf( "[ ok ]\n" );
    } else {
      printf( "[ !! ]\n" );
      die(BD_LOAD_ERROR, item, filename);
    }
}

void
procesar_string (char *valor, char *linea, int *i) {
  /* abcdefghijklmnopqrstuvwxyzáéíóúàèìòùäëïöü */
  Const caracteres : Set of Char = ['a'..'z', 'A'..'Z', 'á'..'ú', 'Á'..'Ú'];

   while( linea [ i ] !in caracteres ) { i++ };
   while(( linea [ i ] in caracteres ) and ( i <= strlen( linea ) ))
     {
      valor += linea[ i ];
      i++;
     }
}

void
procesar_integer (int *valor, char *linea, int *i) {
Const temp : String = '';
    cifras : Set of Char = ['0'..'9'];
    int error;

   while !( linea [ i ] In cifras ) Do i := i + 1;
   while(( linea[ i ] In cifras ) and ( i <= strlen( linea ) )) {
      temp += linea[ i ];
      i++;
     }
   Val ( temp, valor, error );
   if (error <> 0) {
     die(BD_PARSE_ERROR, "", filename , line);
     }
