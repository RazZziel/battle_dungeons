enum element {
    ELEM_WATER,
    ELEM_FIRE,
    ELEM_ROCK
};
enum target {
    TG_PERSON,
    TG_AREA,
    TG_SELF,
    TG_OBJECT
};

struct spell {
    enum element type_element;
    enum target type_target;
    int level;
    int invocation_time;
    int duration;
    int range;
};
