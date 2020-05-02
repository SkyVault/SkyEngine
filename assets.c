#include "assets.h"

Assets* create_and_load_assets() {
    Assets* ass = malloc(sizeof(Assets));

    *ass = (Assets) {
        .meshes = { [MESH_CUBE] = GenMeshCube(1, 1, 1),
					[MESH_SKYBOX] = GenMeshCube(200, 200, 200),
                    [MESH_PLANE] = GenMeshPlane(1, 1, 10, 10), },

        .textures = { [TEX_WALL_1] = LoadTexture("resources/wall_1.png"),
                      [TEX_FLOOR_1] = LoadTexture("resources/floor_1.png"),
                      [TEX_CHAR_1] = LoadTexture("resources/char.png"),
                      [TEX_SALAMI] = LoadTexture("resources/salami.png"),
                      [TEX_PINEAPPLE] = LoadTexture("resources/pineapple.png"),
                      [TEX_EWW] = LoadTexture("resources/eww.png"),
                      [TEX_EWW2] = LoadTexture("resources/eww2.png") },

        .shaders = { [SHADER_PHONG_LIGHTING] = LoadShader(
                        "resources/phong_vs.glsl",
                        "resources/phong_fs.glsl"),
                     [SHADER_SKYBOX] = LoadShader(
                        "resources/skybox_vs.glsl",
                        "resources/skybox_fs.glsl"),
                     [SHADER_CUBEMAP] = LoadShader(
                         "resources/cubemap_vs.glsl",
                         "resources/cubemap_fs.glsl"), }
    };

    for (int i = 0; i < TEX_NUM_TEXTURES; i++) {
        SetTextureFilter(ass->textures[i], FILTER_POINT);
        SetTextureWrap(ass->textures[i], WRAP_REPEAT);
    }

    return ass;
}
