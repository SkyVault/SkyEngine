#ifndef BENIS_ENTITY_TYPES_H
#define BENIS_ENTITY_TYPES_H

#include "utils.h"

#define ENTITY_TYPES(I) \
    I(PLAYER)           \
    I(PINEAPPLE_BOMB)   \
    I(GIRL_1)           \
    I(GIRL_2)           \
    I(GIRL_3)           \
    I(GIRL_4)           \
    I(END_TARGET)       \
    I(NUM_ENTITY_TYPES)

enum EntityTypesE { ENTITY_TYPES(TO_ENUM) };
static const char* EntityTypesS[] = {ENTITY_TYPES(TO_STR)};

#define ACTOR_STATES(I) \
    I(IDLE)             \
    I(TRACK)            \
    I(WAIT)

enum ActorStatesE { ACTOR_STATES(TO_ENUM) };
static const char* ActorStatesS[] = {ACTOR_STATES(TO_STR)};

typedef struct {
    int type;  // entity types defined above
    int state;
} Actor;

Actor create_actor(int type);

#endif  // BENIS_ENTITY_TYPES_H
