#ifndef BENIS_PHYSICS_H
#define BENIS_PHYSICS_H

#include <math.h>
#include <stdlib.h>

#include "game.h"
#include "ecs.h"
#include "raylib.h"
#include "physics_type.h"
#include "map_def.h"

void update_physics(Map* map, EcsWorld* ecs, Game* game, EntId ent);

#endif//BENIS_PHYSICS_H
