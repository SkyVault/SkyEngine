#ifndef BENIS_ASSEMBELR_H
#define BENIS_ASSEMBLER_H

#include "assets.h"
#include "ecs.h"
#include "entity_types.h"
#include "models.h"
#include "physics.h"
#include "player.h"
#include "raylib.h"
#include "utils.h"

typedef EntId (*Ass)(Game*, float, float, float, float);

#define TO_ASS(k) k##_C,

EntId PLAYER_C(Game* game, float x, float y, float vx, float vy);
EntId PINEAPPLE_BOMB_C(Game* game, float x, float y, float vx, float vy);
EntId GIRL_C(Game* game, float x, float y, float vx, float vy);
EntId END_TARGET_C(Game* game, float x, float y, float vx, float vy);
EntId NUM_ENTITY_TYPES_C(Game* game, float x, float y, float vx, float vy);

static const Ass assemblers[NUM_ENTITY_TYPES + 1] = {ENTITY_TYPES(TO_ASS)};

EntId assemble(int which, Game* game, float x, float y, float vx, float vy);

#endif  // BENIS_ASSEMBLER_H