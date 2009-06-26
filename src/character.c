#include "global.h"

//objetos
//armas
//coordenadas

//#include "files.h"

//pj_t *jugador;

/*
void procesar_personaje (*char pjln, pj *jugador) {
  int i = 0; // Posicion en pjln
  procesar_*char[]( pj.nombre, pjln, i );
  procesar_int( pj.fue, pjln, i );
  procesar_int( pj.des, pjln, i );
  procesar_int( pj.con, pjln, i );
  procesar_int( pj.int, pjln, i );
  procesar_int( pj.sab, pjln, i );
  procesar_int( pj.car, pjln, i );
  //
}
*/
/*   
void buscar_raza (pj_t pj, bool seleccion) {
  bool encontrado  = false;
  int i;
  *char pjln; 
  *char nombre;
  
  Reset ( bd_razas );
  printf( 'Raza: ' ); read( nombre );
  nombre = nombre + ' ';

  do { // Archivo
    read ( bd_razas, pjln );
    i = 0;
    
    do {// linea
      i++;
    } until ( pjln[i]!=nombre[i] ) || (i==strlen(nombre) ); // lLinea
    
    if ( (i==strlen(nombre)) && ( pjln [i]==nombre[i]) )
      encontrado = true;
    
    until (encontrado || EoF( bd_razas )); // /Archivo
    
    if encontrado {
      procesar_personaje ( pjln, pj );
    } else {
      printf( 'Los %ss parece que se esconden...', nombre );
      seleccion = false;
    }

  }
}
*/
/*
void raza_aleatoria (pj_t pj) { //Determinar el numero de razas de la BD
int contador_razas = 0;
int i;
*char[] pjln;
*char[] temp;

Reset ( bd_razas );
While Not EoF ( bd_razas ) DO
Begin
      contador_razas := contador_razas + 1;
      ReadLn ( bd_razas );
   End;
   { Seleccionar una raza aleatoriamente }
   Reset( bd_razas );
   for ( i=1; i<=(random(contador_razas)-1) )
   {
      readln( bd_razas );
   }
   readln( bd_razas, pjln );
   procesar_personaje( pjln, pj );
}

{
   cargar_bd( bd_razas, BD_razas_NOMBRE, 'razas' );
   cargar_bd( bd_clases, BD_clases_NOMBRE, 'clases' );
}
*/
