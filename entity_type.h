#ifndef BENIS_ENTITY_TYPES_H
#define BENIS_ENTITY_TYPES_H

#include "utils.h"

#define ENTITY_TYPES(I)     \
    I(ACTOR_PLAYER)         \
    I(ACTOR_PINEAPPLE_BOMB) \
    I(ACTOR_TOSSED_ORANGE)  \
    I(ACTOR_GIRL_1)         \
    I(ACTOR_GIRL_2)         \
    I(ACTOR_GIRL_3)         \
    I(ACTOR_GIRL_4)         \
    I(ACTOR_END_TARGET)     \
    I(ACTOR_BLOCK)          \
    I(ACTOR_NUM_ENTITY_TYPES)

enum EntityTypesE { ENTITY_TYPES(TO_ENUM) };
static const char* EntityTypesS[] = {ENTITY_TYPES(TO_STR)};

#define ACTOR_STATES(I) \
    I(IDLE)             \
    I(TRACK)            \
    I(WAIT)

enum ActorStatesE { ACTOR_STATES(TO_ENUM) };
static const char* ActorStatesS[] = {ACTOR_STATES(TO_STR)};

enum Actor_Flags {
    AF_NONE = 1 << 0,
    AF_ENEMY = 1 << 1,
};

typedef struct {
    int type;  // entity types defined above
    int state;

    int flags;

    float health;
} Actor;

Actor create_actor(int type);
Actor create_enemy_actor(int type);

#endif  // BENIS_ENTITY_TYPES_H
