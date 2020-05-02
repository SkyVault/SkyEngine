#ifndef BENIS_ASSETS_H
#define BENIS_ASSETS_H

#include <stdlib.h>

#include "raylib.h"

enum Textures {
    TEX_WALL_1,
    TEX_FLOOR_1,
    TEX_CHAR_1,
    TEX_SALAMI,
    TEX_PINEAPPLE,
    TEX_EWW,
    TEX_EWW2,
    TEX_NUM_TEXTURES,
};

enum Meshes {
    MESH_CUBE,
    MESH_SKYBOX,
    MESH_PLANE,
    MESH_NUM_MESHES
};

enum Shaders {
    SHADER_PHONG_LIGHTING,
    SHADER_SKYBOX,
    SHADER_CUBEMAP,
    SHADER_NUM_SHADERS,
};

typedef struct {
    Mesh meshes[MESH_NUM_MESHES];
    Texture2D textures[TEX_NUM_TEXTURES];
    Shader shaders[SHADER_NUM_SHADERS];
} Assets;

Assets* create_and_load_assets();

#endif//BENIS_ASSETS_H
