/* Módulo de mounstruos, criaturas y cobradores de Hacienda de   *
 * Battle Dungeons.                                              *
 * El módulo carga la base de datos de enemigos desde un archivo *
 * externo de texto, y gestiona cada uno de los animalillos.     */

#include "coordenadas.h"
#include "ficheros.h"
#include "randomizations.h"

FILE *bd_enemigos;
char enemigoln[50];

void enemigos_init()
{
  load_bd( bd_enemigos, BD_ENEMIGOS_NOMBRE, "enemigos" );
}

void procesar_enemigo (char enemigoln[40];
		       struct monster enemigo;
char i 0;  { Posicion en enemigoln }
{
   procesar_str( enemigo.nombre, enemigoln, i );
   procesar_int( enemigo.fue,    enemigoln, i );
   procesar_int( enemigo.des,    enemigoln, i );
   procesar_int( enemigo.con,    enemigoln, i );
   procesar_int( enemigo.int,    enemigoln, i );
   procesar_int( enemigo.sab,    enemigoln, i );
   procesar_int( enemigo.car,    enemigoln, i );
   //
}
   
void buscar_enemigo (monster enemigo;
		     boolean seleccion);
boolean encontrado false;
int  i;  
char enemigoln[40], nombre[30];
{
   Reset( bd_enemigos );
   printf( "Nombre del enemigo: " ); scanf( "%c", nombre );
   nombre = nombre + ' ';
   do                                                                         //  Archivo
     {
       ReadLn( bd_enemigos, enemigoln );
       i = 0;

       do                                                                     //  Linea
	 i = i + 1;
       while (( enemigoln [i] = nombre [i] ) and ( i < Length ( nombre ) )); // /Linea

       if ( ( i = Length ( nombre ) ) and ( enemigoln [i] = nombre [i] ))
	 encontrado = true;
     }
   while (!encontrado and !EoF( bd_enemigos ));                               // /Archivo
   
   if( encontrado )
     procesar_enemigo( enemigoln, enemigo )
       else
	 {
	   printf( "%c parece que se esconde...\n", nombre );
	   seleccion = false;
	 }
}

monster enemigo_aleatorio();
{
  int contador_enemigos = 0;
  int i;
  char enemigoln[40], temp[40];

  // Determinar el numero de enemigos de la BD
   Reset( bd_enemigos );
   while (!EoF( bd_enemigos ))
   {
      contador_enemigos = contador_enemigos + 1;
      ReadLn( bd_enemigos );
   }

   // Seleccionar un enemigo aleatoriamente
   Reset( bd_enemigos );
   for( i=0, i<( random( contador_enemigos ) - 1 ), i++)
      ReadLn( bd_enemigos );
   ReadLn( bd_enemigos, enemigoln );
   procesar_enemigo( enemigoln, enemigo );
}
