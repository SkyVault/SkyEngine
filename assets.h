#ifndef BENIS_ASSETS_H
#define BENIS_ASSETS_H

#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"
#include "rlights.h"

enum Textures {
    TEX_WALL_1,
    TEX_WALL_2,
    TEX_WALL_3,

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

    // Terrain Textures
    TEX_GRASS_1,

    TEX_NUM_TEXTURES,
};

enum Meshes { MESH_CUBE, MESH_SKYBOX, MESH_PLANE, MESH_NUM_MESHES };

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

    Model* models;
    int num_models;

    Light lights[MAX_LIGHTS];
    int num_lights;
    Sun sun;
} Assets;

Shader hotload_shader(Assets* self, const char* path_vs, const char* path_fs,
                      int id);

Assets* create_and_load_assets();

// Used for hotloading and dynamic recomp
void update_assets(Assets* self);

#endif  // BENIS_ASSETS_H
