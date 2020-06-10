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
#include "map_type.h"
#include "physics_type.h"
#include "raylib.h"
#include "raymath.h"
#include "rlights.h"

/* NOTE(Dustin):
    The map shouldn't contain any assets itself, but should request the
    asset manager to make sure the assets that it neads, is loaded. Then
    the map can make requests to the asset manager for all of the needed
    models and textures
*/

static const Prop prop_types[] = {
    {.region = {0, 0, 419, 420}, .position = {0, 0, 0}, .scale = 1.0f},
    {.region = {420, 0, 419, 420}, .position = {0, 0, 0}, .scale = 1.0f},
    {.region = {840, 0, 419, 420}, .position = {0, 0, 0}, .scale = 1.0f},
};

Map *create_map_from_script(const char *path, Game *game);

void load_map_from_script(Map *map, const char *path, Game *game);

void reset_map_to_zero(Map *map, Game *game);
void reload_map(Map *map, Game *game);

void update_map(Map *map, Game *game);
void render_map(Map *map, GfxState *gfx, Game *game);

bool add_exit(Map *map, Vector3 position, int id, int dest_id,
              const char *dest_path);

bool add_actor_spawn(Map *self, int type, Vector3 position);

#endif  // BENIS_MAP_H
