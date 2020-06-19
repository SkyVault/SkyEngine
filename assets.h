#ifndef BENIS_ASSETS_H
#define BENIS_ASSETS_H

#include <janet.h>
#include <stdio.h>
#include <stdlib.h>

#include "dict.h"
#include "raylib.h"
#include "raymath.h"
#include "rlights.h"

#define ATLAS_SIZE 1024
#define ATLAS_TILE_SIZE 256
#define ATLAS_TILES_WIDE (ATLAS_SIZE / ATLAS_TILE_SIZE)

enum Textures {
    TEX_WALL_1,
    TEX_WALL_2,
    TEX_WALL_3,
    TEX_WALL_4,
    TEX_WALL_5,
    TEX_WALL_6,

    TEX_WALL_ATLAS,

    TEX_PROPS,

    TEX_CHAINLINK_FENCE,

    TEX_FLOOR_1,
    TEX_CHAR_1,

    TEX_SALAMI,

    TEX_PINEAPPLE,
    TEX_ORANGE,

    TEX_GIRL_1,
    TEX_GIRL_2,
    TEX_GIRL_3,
    TEX_GIRL_4,

    TEX_BARREL,

    // Terrain Textures
    TEX_GRASS_1,

    TEX_NUM_TEXTURES,
};

enum Scripts { SCRIPTS_BASIC_ZOMBIE_AI, SCRIPTS_NUM_SCRIPTS };

enum Meshes {
    MESH_CUBE,
    MESH_SKYBOX,
    MESH_PLANE,
    MESH_BARREL,
    MESH_NUM_MESHES
};

enum { SHADER_VERTEX, SHADER_FRAGMENT };

enum Shaders {
    SHADER_PHONG_LIGHTING,
    SHADER_PHONG_2_0_LIGHTING,
    SHADER_SKYBOX,
    SHADER_CUBEMAP,
    SHADER_NUM_SHADERS,
};

enum Fonts {
    FONT_MAIN_FONT,
    FONT_NUM_FONTS,
};

struct Hotload {
    bool hotload;
    long last_time_modified[2];
    const char* paths[2];

    bool just_hotloaded;
};

typedef struct {
    Mesh meshes[MESH_NUM_MESHES];
    Texture2D textures[TEX_NUM_TEXTURES];
    Shader shaders[SHADER_NUM_SHADERS];

    struct Hotload shaders_hotload[SHADER_NUM_SHADERS];

    Font fonts[FONT_NUM_FONTS];

    JanetFunction* scripts[SCRIPTS_NUM_SCRIPTS];

    Model* models;
    int num_models;

    Dict* models_dict;

    Light lights[MAX_LIGHTS];
    int num_lights;
    Sun sun;
} Assets;

static Model* alloc_model_from(Model model) {
    Model* result = (Model*)malloc(sizeof(Model));
    *result = model;
    return result;
}

Shader hotload_shader(Assets* self, const char* path_vs, const char* path_fs,
                      int id);

Assets* create_and_load_assets(void);

void assets_load_scripts(Assets* self, JanetTable* env);

// Used for hotloading and dynamic recomp
void update_assets(Assets* self);

#endif  // BENIS_ASSETS_H
