#ifndef CHARACTER_H
#define CHARACTER_H


/* Actions */

typedef void action_t;

/* Entities */

typedef enum {
    ENTITY_WHATEVER,
    ENTITY_PC,
    ENTITY_NPC,
    ENTITY_OBJECT
} entity_type_t;

typedef enum {
    OBJECT_WHATEVER,
    OBJECT_WEAPON,
    OBJECT_ARMOR,
    OBJECT_DRUG
} object_type_t;

typedef enum {human, dwraf, elf, gnome, half_elf, half_orc, halfling}
    race_t;
typedef enum {male, female}
    gender_t;
typedef enum {barbarian, bard, cleric, druid, fighter, monk, paladin, ranger, rogue, sorcerer, wizard}
    class_t;
typedef enum {lawful, neutral1, chaotic}
    alignment_t1;
typedef enum {good, neutral2, evil}
    alignment_t2;
typedef enum {head, body, arms, hands, legs, feet, right_hand, left_hand}
    places_t;

typedef struct {
    int str, dex;
} weapon_t;

typedef struct {
    object_type_t type;

    int price;

    /* Subclasses */
    union {
        weapon_t *weapon;
    } data;
} object_t;

typedef struct {
    gender_t gender;
    race_t race;
    class_t class_;
    alignment_t1 alignment1;
    alignment_t2 alignment2;

    char deity;
    int age;
    int height;
    int weight;

    int str;            /* strength     */
    int dex;            /* dexterity    */
    int con;            /* constitution */
    int intl;           /* intelligence */
    int wiz;            /* wisdom       */
    int cha;            /* charisma     */

    int hp, hp_max;
    int mp, mp_max;
    int size;
    int speed;

    int exp;
    int level;
    int range_sight;
    bool aggressive;

    object_t *objects;
    int n_objects;
    int money;

    struct {
        bool alive;
        bool poissoned;
        bool drunk;
        bool blind;
    } status;
} character_t;

typedef struct {
    entity_type_t type;

    int x, y;
    int color;
    char tile;
    char *name;

    /* Events */
    struct {
        action_t *on_interact;
        action_t *on_touch;
    } actions;

    /* Subclasses */
    union {
        character_t *character;
        object_t *object;
    } data;
} entity_t;

/* Dices */

typedef struct {
    char number;
    char value;
} dices;



#endif /* CHARACTER_H */
