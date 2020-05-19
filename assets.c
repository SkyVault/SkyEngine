#include "assets.h"

Assets* create_and_load_assets() {
    Assets* ass = malloc(sizeof(Assets));

    *ass = (Assets){
        .meshes =
            {
                [MESH_CUBE] = GenMeshCube(1, 1, 1),
                [MESH_SKYBOX] = GenMeshCube(200, 200, 200),
                [MESH_PLANE] = GenMeshPlane(1, 1, 10, 10),
            },

        .textures =
            {
                [TEX_WALL_1] = LoadTexture("resources/wall_1.png"),
                [TEX_WALL_2] = LoadTexture("resources/wall_2.png"),
                [TEX_WALL_3] = LoadTexture("resources/wall_3.png"),
                [TEX_PROPS] = LoadTexture("resources/props.png"),
                [TEX_CHAINLINK_FENCE] = LoadTexture("resources/chainlink.png"),
                [TEX_FLOOR_1] = LoadTexture("resources/floor_1.png"),
                [TEX_CHAR_1] = LoadTexture("resources/char.png"),
                [TEX_SALAMI] = LoadTexture("resources/salami.png"),
                [TEX_PINEAPPLE] = LoadTexture("resources/pineapple.png"),
                [TEX_ORANGE] = LoadTexture("resources/orange.png"),
                [TEX_GIRL_1] = LoadTexture("resources/girls/girl_1.png"),
                [TEX_GIRL_2] = LoadTexture("resources/girls/girl_2.png"),
                [TEX_GIRL_3] = LoadTexture("resources/girls/girl_3.png"),
                [TEX_GIRL_4] = LoadTexture("resources/girls/girl_4.png"),
                [TEX_GRASS_1] = LoadTexture("resources/textures/grass_1.png"),
            },

        .shaders =
            {
                [SHADER_PHONG_LIGHTING] = LoadShader("resources/phong_vs.glsl",
                                                     "resources/phong_fs.glsl"),
                [SHADER_SKYBOX] = LoadShader("resources/skybox_vs.glsl",
                                             "resources/skybox_fs.glsl"),
                [SHADER_CUBEMAP] = LoadShader("resources/cubemap_vs.glsl",
                                              "resources/cubemap_fs.glsl"),
            },

        .fonts = {
            [FONT_MAIN_FONT] = LoadFont("resources/alagard_font.png"),
        }};

    for (int i = 0; i < TEX_NUM_TEXTURES; i++) {
        SetTextureFilter(ass->textures[i], FILTER_POINT);
        SetTextureWrap(ass->textures[i], WRAP_REPEAT);
    }

    return ass;
}
