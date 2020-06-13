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
#include "gameworld_types.h"
#include "physics_type.h"
#include "raylib.h"
#include "raymath.h"
#include "renderer.h"
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

Region *create_region_from_script(const char *path, Game *game);

void load_region_from_script(Region *self, const char *path, Game *game);

void reset_region_to_zero(Region *self, Game *game);
void reload_region(Region *self, Game *game);

void update_region(Region *self, Game *game);
void render_region(Region *self, GfxState *gfx, Game *game);

bool add_exit(Region *self, Vector3 position, int id, int dest_id,
              const char *dest_path);

bool add_actor_spawn(Region *self, int type, Vector3 position);

#endif  // BENIS_MAP_H
