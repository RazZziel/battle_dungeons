Autores:                                             ∧∧
        Ismael Barros Barros                     　( ﾟ∀ﾟ)
        Daniel Gómez Ferro                        ⊂　　つ
                                                 　(つ ﾉ
                                                  　(ノ
Contenido:
        battle_dungeons/src              :  código fuente
        battle_dungeons/src/scripting.l  :  lexer
        battle_dungeons/src/scripting.y  :  parser
        battle_dungeons/src/scripting.c  :  manejo del intérprete
        battle_dungeons/include          :  estructuras de datos
        battle_dungeons/data             :  scripts de ejemplo

Compilación:                    Ejecución:
        cd battle_dungeons              cd battle_dungeons/src
        ./autogen.sh                    ./battle_dungeons
        ./configure
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

        TODO:
            - Acciones, que son pequeños procedimientos implementados
              en un sencillo lenguaje, y que se pueden ser asociadas
              a eventos en mapas o entidades, para que por ejemplo al
              interactuar con un guardia se pueda comenzar una
              conversación, que al pisar una trampa en el mapa ésta
              se active, o simplemente que al pasar por una puerta el
              jugador se teletransporte a otro mapa.

        FIXME:
            - Limpiar todos los pig disgusting hacks.



[1] http://en.wikipedia.org/wiki/Roguelike
