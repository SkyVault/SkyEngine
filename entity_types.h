#ifndef BENIS_ENTITY_TYPES_H
#define BENIS_ENTITY_TYPES_H

#include "utils.h"

#define ENTITY_TYPES(I) \
    I(PLAYER)           \
    I(PINEAPPLE_BOMB)   \
    I(GIRL)             \
    I(END_TARGET)       \
    I(NUM_ENTITY_TYPES)

enum EntityTypesE { ENTITY_TYPES(TO_ENUM) };
static const char* EntityTypesS[] = {ENTITY_TYPES(TO_STR)};

typedef struct {
    int type;  // Defined above
} Actor;

Actor create_actor(int type);

#endif  // BENIS_ENTITY_TYPES_H
