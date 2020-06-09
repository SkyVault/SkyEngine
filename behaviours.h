#ifndef BENIS_BEHAVIOURS_H
#define BENIS_BEHAVIOURS_H

#include "components.h"
#include "ecs.h"
#include "entity_type.h"
#include "game.h"
#include "raymath.h"
#include "scripting.h"

void register_janet_c_functions(Game* game);

void update_behaviours(Game* game, EcsWorld* ecs, EntId id);

#endif  // BENIS_BEHAVIOURS_H