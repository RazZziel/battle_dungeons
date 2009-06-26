Autores:                                             ∧∧
        Ismael Barros Barros                     　( ﾟ∀ﾟ)
        Daniel Gómez Ferro                        ⊂　　つ
                                                 　(つ ﾉ
                                                  　(ノ
Contenido:
        battle_dungeons/src               :  código fuente
        battle_dungeons/src/parser.l      :  lexer
        battle_dungeons/src/parser.y      :  parser
        battle_dungeons/src/parser.c      :  API del parser
        battle_dungeons/src/interpreter.c :  intérprete, AST
        battle_dungeons/include           :  estructuras de datos
        battle_dungeons/data              :  scripts de ejemplo

Compilación:                    Ejecución:
        cd battle_dungeons              cd battle_dungeons
        ./autogen.sh                    cd src
        ./configure                     ./battle_dungeons
        make

Comentarios:
        Esta práctica está montada sobre un intento de motor de
        roguelike[1] (una especie de juego de rol en modo texto)
        que tenía empezado.

        Los objetivos de la práctica son:

            - Dotar al juego de rol de capacidades de scripting,
              para poder definir nuevos mapas, entidades y
              comportamientos sin tener que recompilar, mediante
              scripts en texto plano interpretados en tiempo de
              ejecución.

            - Refactorizar el motor del juego para soportar las
              nuevas funcionalidades definidas en los scripts.


        Capacidades de los scripts a día de hoy:

            - Mapas, cada uno con varias capas superpuestas de tiles
              ("baldosas"), entidades o eventos. Los mapas llevan
              asociados una serie de materiales, que son los que
              definen la apariencia gráfica de cada tile. Ahora mismo
              sólo está implementado el soporte para juegos en modo
              texto, pero en teoría se podrían soportar mapas gráficos
              bidimensionales o tridimensionales sin mucho esfuerzo.
                  ejemplo:   battle_dungeons/data/maps.rpg
                             battle_dungeons/data/materials.rpg

            - Entidades, que pueden ser objetos o personajes, y son
              definidos de maneja jerárquica, con herencia múltiple.
              Cada entidad hereda una serie de propiedades de sus
              "superclases", que puede sobreescribir o ampliar
                  ejemplo:   battle_dungeons/data/entities.rpg

            - Acciones, que son pequeños procedimientos implementados
              en un sencillo lenguaje, y que se pueden ser asociadas
              a eventos en mapas o entidades, para que por ejemplo al
              interactuar con un guardia se pueda comenzar una
              conversación, que al pisar una trampa en el mapa ésta
              se active, o simplemente que al pasar por una puerta el
              jugador se teletransporte a otro mapa.
                 ejemplo:   battle_dungeons/data/---------------
              Esta parte todavía está muy verde; sólo se ha
              implementado lo básico para obtener una prueba de
              concepto funcional.

              Detalles de diseño del lenguaje:

                Se trata de un sencillo lenguaje multiparadigma
              con tipado dinámico. Todas las componentes del
              lenguaje son una expresión, para simplificar el diseño
              y maximizar la flexibilidad, y se puede emplear tanto
              un diseño algorítmico funcional muy limitado como uno
              imperativo. Aunque toda expresión tiene un tipo único,
              su control se realiza únicamente en tiempo de ejecución,
              para simplificar el intérprete y agilizar la tarea del
              programador.


Nota:
        Conscientes de que el hecho de que el código esté en inglés puede
        despertar recelos sobre su originalidad, informamos que el repositorio
        que almacena el historial reciente de la aplicación se encuentra
        disponible públicamente en:
                   http://github.com/RazZziel/battle_dungeons
        y pedimos disculpas por la probable poca adecuación de los mensajes de
        commit y de muchos de los comentarios del código.


[1] http://en.wikipedia.org/wiki/Roguelike
