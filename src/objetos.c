#include <ficheros.h>;

#define BD_OBJETOS_NOMBRE "Data/objetos.db";

Type
   objeto	= Record
		     nombre	 : String;
		     descripcion : String;
		     precio	 : Integer; { En 'cu' }
		     peso	 : Integer;
		     cantidad	 : Integer;
		  End;
   pt_objeto	= ^node_objetos;
   node_objetos	= Record
		     data : objeto;
		     next : pt_objeto;
		  End;	  
Var
   inventario : pt_objeto;
   bd_objetos : Text;
   
Procedure Importar_Objeto (    nombre	  : String;
			   VAR objeto	  : pt_objeto;
			   VAR encontrado : Boolean);
Var i	    : Integer;
   objetoln : String;
   Procedure Procesar_Objeto (objetoln : String;
			      objeto   : pt_objeto);
   Begin
   End;
Begin
   encontrado := false;
   Reset ( bd_objetos );
   Repeat { Archivo }
      ReadLn ( bd_objetos, objetoln );
      i := 0;
      Repeat {Linea}
	 i := i + 1;
      Until ( objetoln [i] <> nombre [i] ) or ( i = Length (nombre ) ); {/Linea}
      If ( i = Length ( nombre ) ) and ( objetoln [i] = nombre [i] ) Then
	 encontrado := true;
   Until encontrado or EoF ( bd_objetos ); { /Archivo }
   If encontrado Then
      Procesar_objeto ( objetoln, objeto );
End; (* Importar_Objeto *)

Procedure Nuevo_Objeto (nombre : String);
Var objeto : pt_objeto;
existente  : Boolean;
Begin
   New (objeto);
   Importar_Objeto (nombre, objeto, existente);
   If existente Then
   Begin
      objeto^.next := inventario;
      inventario := objeto;
   End
   Else
   Begin
      WriteLn ( '*** ERROR: No se encuentra el objeto en la BD. ***' );
      WriteLn ( '***   Comunica el bug a razielmine@gmail.com   ***' );
   End   
End; (* Nuevo Objeto *)

Procedure Quitar_Objeto (nombre : String);
Begin
   Buscar_Objeto (nombre);
End; (* Quitar_Objeto *)

Procedure Buscar_Objeto (buscado : String);
Const encontrado : Boolean = false;
Var cursor : pt_objeto;   
Begin
   cursor := inventario;
   Repeat
      If cursor^.data.nombre = buscado Then
	 encontrado := true
      Else
	 cursor := cursor^.next;
   Until ( encontrado ) or ( cursor = nil );
End; (* Buscar_Objeto *)

Begin
   Cargar_BD ( bd_objetos, BD_OBJETOS_NOMBRE, 'objetos' );
   New ( inventario );
   inventario := nil;
End.
