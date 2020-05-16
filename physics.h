#ifndef BENIS_PHYSICS_H
#define BENIS_PHYSICS_H

#include <math.h>
#include <stdlib.h>

#include "ecs.h"
#include "game.h"
#include "map_type.h"
#include "physics_type.h"
#include "raylib.h"

void update_physics(Map* map, EcsWorld* ecs, Game* game, EntId ent);

#endif  // BENIS_PHYSICS_H
