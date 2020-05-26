#ifndef BENIS_ASSETS_H
#define BENIS_ASSETS_H

#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

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

typedef struct {
    Mesh meshes[MESH_NUM_MESHES];
    Texture2D textures[TEX_NUM_TEXTURES];
    Shader shaders[SHADER_NUM_SHADERS];
    Font fonts[FONT_NUM_FONTS];

    Model* models;
    int num_models;
} Assets;

Assets* create_and_load_assets();

#endif  // BENIS_ASSETS_H
