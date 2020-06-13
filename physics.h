#ifndef BENIS_PHYSICS_H
#define BENIS_PHYSICS_H

#include <math.h>
#include <stdlib.h>

#include "ecs.h"
#include "game.h"
#include "gameworld_types.h"
#include "physics_type.h"
#include "raylib.h"

void update_physics(Region* map, EcsWorld* ecs, Game* game, EntId ent);

#endif  // BENIS_PHYSICS_H
