#ifndef BENIS_MAP_H
#define BENIS_MAP_H

#include <ctype.h>
#include <janet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "assets.h"
#include "components.h"
#include "ecs.h"
#include "game.h"
#include "map_def.h"
#include "physics_type.h"
#include "raylib.h"
#include "raymath.h"

Map *load_map_from_file(const char *path, Game *game);
Map *load_map_from_script(const char *path, Game *game);

void destroy_map(Map *map, Game *game);
void reload_map(Map *map, Game *game);

void update_map(Map *map, Game *game);
void draw_map(Map *map, Game *game);

#endif  // BENIS_MAP_H
