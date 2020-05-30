#ifndef BENIS_MAP_DEF_H
#define BENIS_MAP_DEF_H

#include <inttypes.h>

#include "constants.h"
#include "prop.h"
#include "raylib.h"
#include "rlights.h"

#define CUBE_SIZE GLOBAL_SCALE
#define CUBE_HEIGHT (CUBE_SIZE * 2)

#define MAX_MAP_WIDTH (100)
#define MAX_MAP_HEIGHT (100)

#define MAX_NUM_LAYERS (10)
#define MAX_MODELS (100)
#define MAX_PROPS (10000)
#define MAX_ACTOR_SPAWNS (100)
#define MAX_DOORS (100)

typedef struct {
    uint8_t active;
    uint8_t model;
} Wall;

typedef struct {
    int type;
    Vector3 position;
} ActorSpawn;

typedef struct {
    Vector3 position;
    int id;
    int dest_id;
    char* dest_path;
} Exit;

typedef struct {
    int current_map;
    int num_layers;

    Wall walls[MAX_NUM_LAYERS][MAX_MAP_WIDTH * MAX_MAP_HEIGHT];

    Model floor_tile_models[1];
    Model models[MAX_MODELS];
    Prop props[MAX_PROPS];
    Exit exits[MAX_DOORS];

    ActorSpawn spawns[MAX_ACTOR_SPAWNS];

    int num_models;
    int num_props;
    int num_spawns;
    int num_exits;

    int width;
    int height;

    float player_x;
    float player_z;

    struct {
        char* buff;
        size_t len;
    } path;
} Map;

#endif  // BENIS_MAP_DEF_H
