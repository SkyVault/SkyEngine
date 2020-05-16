#ifndef BENIS_MAP_DEF_H
#define BENIS_MAP_DEF_H

#include <inttypes.h>

#include "prop.h"
#include "raylib.h"
#include "rlights.h"

#define CUBE_SIZE (5)
#define CUBE_HEIGHT (CUBE_SIZE * 2)

#define MAX_MAP_WIDTH (100)
#define MAX_MAP_HEIGHT (100)

#define MAX_NUM_LAYERS (10)
#define MAX_MODELS (100)
#define MAX_PROPS (10000)
#define MAX_ACTOR_SPAWNS (100)

typedef struct {
    uint8_t active;
    uint8_t model;
} Wall;

typedef struct {
    int type;
    Vector2 position;
} ActorSpawn;

typedef struct {
    int current_map;
    int num_layers;

    Wall walls[MAX_NUM_LAYERS][MAX_MAP_WIDTH * MAX_MAP_HEIGHT];

    Model floor_tile_models[1];
    Model models[MAX_MODELS];
    Prop props[MAX_PROPS];
    Light lights[MAX_LIGHTS];

    int light_color[MAX_LIGHTS];

    ActorSpawn spawns[MAX_ACTOR_SPAWNS];

    int num_models;
    int num_props;
    int num_lights;
    int num_spawns;

    int width;
    int height;

    float player_x;
    float player_y;

    struct {
        char* buff;
        size_t len;
    } path;
} Map;

#endif  // BENIS_MAP_DEF_H
