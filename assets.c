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

    Shader* shader = &ass->shaders[SHADER_PHONG_LIGHTING];

    shader->locs[LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");
    shader->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");

    ass->sun = CreateSun(*shader, (Vector3){-1.2f, -1.0f, -0.3},
                         (Color){0, 0, 0, 255}, (Color){200, 200, 200, 255});

    // Initialize the lights
    for (int i = 0; i < MAX_LIGHTS; i++) {
        ass->lights[i] = CreateLight(LIGHT_POINT, Vector3Zero(), Vector3Zero(),
                                     WHITE, *shader);
        ass->lights[i].enabled = false;
        UpdateLightValues(*shader, ass->lights[i]);
    }

    for (int i = 0; i < TEX_NUM_TEXTURES; i++) {
        SetTextureFilter(ass->textures[i], FILTER_POINT);
        SetTextureWrap(ass->textures[i], WRAP_REPEAT);
    }

    ass->models = malloc(sizeof(Model) * 6);
    ass->num_models = 6;

    Model default_wall = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    default_wall.materials[0].maps[MAP_DIFFUSE].texture =
        ass->textures[TEX_WALL_1];
    default_wall.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

    Model default_wall_2 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    default_wall_2.materials[0].maps[MAP_DIFFUSE].texture =
        ass->textures[TEX_WALL_2];
    default_wall_2.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

    Model default_wall_3 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    default_wall_2.materials[0].maps[MAP_DIFFUSE].texture =
        ass->textures[TEX_WALL_3];
    default_wall_2.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

    Model fence_1 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    fence_1.materials[0].maps[MAP_DIFFUSE].texture =
        ass->textures[TEX_CHAINLINK_FENCE];
    fence_1.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];
    fence_1.transform = MatrixIdentity();
    fence_1.transform =
        MatrixMultiply(fence_1.transform, MatrixScale(0.01f, 1.0f, 1.0f));

    Model fence_2 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    fence_2.materials[0].maps[MAP_DIFFUSE].texture =
        ass->textures[TEX_CHAINLINK_FENCE];
    fence_2.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];
    fence_2.transform = MatrixIdentity();
    fence_2.transform =
        MatrixMultiply(fence_2.transform, MatrixScale(1.0f, 1.0f, 0.01f));

    Model floor_1 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
    floor_1.materials[0].maps[MAP_DIFFUSE].texture = ass->textures[TEX_FLOOR_1];
    floor_1.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

    ass->models[0] = default_wall;
    ass->models[1] = default_wall_2;
    ass->models[2] = default_wall_3;
    ass->models[3] = fence_1;
    ass->models[4] = fence_2;
    ass->models[5] = floor_1;

    return ass;
}
