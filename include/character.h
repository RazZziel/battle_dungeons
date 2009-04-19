#ifndef CHARACTER_H
#define CHARACTER_H

enum race_t {human, dwraf, elf, gnome, half_elf, half_orc, halfling};
enum gender_t {male, female};
enum class_t {barbarian, bard, cleric, druid, fighter, monk, paladin, ranger, rogue, sorcerer, wizard};
enum alignment_t1 {lawful, neutral1, chaotic};
enum alignment_t2 {good, neutral2, evil};

struct inventory_t {
  
// struct {
//    struct {
//      weapon right_hand;
//      weapon left_hand;
//    } held;
//    weapon_list inv;
//  } weapons;

//  struct {
//    struct {
//      list head;
//      list body;
//      list arms;
//      list hands;
//      list legs;
//      list feet;
//    } worn;
//    list inv;
//  } armor;
  
//  list objects;

    struct {
        int cp;   /* cobre   */
        int sp;   /* plata   */
        int gp;   /* oro     */
        int pp;   /* platino */
    } money;
  
};



typedef struct {
    int playable;
	
    char *name;                    /* nombre               */
    char *prefix;                  /* articulo del nombre  */
    int color;
    char tile;
    enum gender_t gender;          /* genero               */
    enum race_t race;              /* raza                 */
    enum class_t class_;           /* clase                */
    enum alignment_t1 alignment1;  /* alineamiento (1)     */
    enum alignment_t2 alignment2;  /* alineamiento (2)     */
    char deity;                    /* deidad               */
    int age;                       /* edad                 */
    int height;                    /* altura               */
    int weight;                    /* peso                 */

    int str;                       /* fuerza               */
    int dex;                       /* destreza             */
    int con;                       /* constitucion         */
    int intl;                      /* inteligencia         */
    int wiz;                       /* sabiduria            */
    int cha;                       /* carisma              */

    int size;                      /* tamanyo              */
    int hp;                        /*                      */
    int hp_max;                    /*                      */
    int mp;                        /* puntos de magia      */
    int mp_max;                    /* puntos de magia      */
    int speed;                     /* velocidad            */
    struct {
        bool alive;
        bool poissoned;
        bool drunk;
        bool blind;
    } status;                      /* estado               */
    int exp;                       /* experiencia          */
    int level;                     /* nivel                */
    struct inventory_t equipment;  /* inventario           */
    int x;                         /* situacion en el mapa */
    int y;                         /* situacion en el mapa */
    int range_sight;
} pc_t;

typedef struct {
    char number;
    char value;
} dices;



#endif /* CHARACTER_H */
