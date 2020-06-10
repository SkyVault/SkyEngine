#include "assets.h"

#define SUN_DIRECTION ((Vector3){10.2f, -20.0f, 10.3})

Shader hotload_shader(Assets* self, const char* path_vs, const char* path_fs,
                      int id) {
    Shader result = LoadShader(path_vs, path_fs);

    self->shaders_hotload[id].hotload = true;
    self->shaders_hotload[id].paths[SHADER_VERTEX] = path_vs;
    self->shaders_hotload[id].paths[SHADER_FRAGMENT] = path_fs;
    self->shaders_hotload[id].last_time_modified[SHADER_VERTEX] =
        GetFileModTime(path_vs);
    self->shaders_hotload[id].last_time_modified[SHADER_FRAGMENT] =
        GetFileModTime(path_fs);

    return result;
}

Assets* create_and_load_assets(void) {
    Assets* ass = malloc(sizeof(Assets));

    for (int i = 0; i < SCRIPTS_NUM_SCRIPTS; i++) {
        ass->scripts[i] = NULL;
    }

    for (int i = 0; i < SHADER_NUM_SHADERS; i++) {
        ass->shaders_hotload[i].hotload = false;
        ass->shaders_hotload[i].just_hotloaded = false;
    }

    ass->meshes[MESH_CUBE] = GenMeshCube(1, 1, 1);
    ass->meshes[MESH_SKYBOX] = GenMeshCube(200, 200, 200);
    ass->meshes[MESH_PLANE] = GenMeshPlane(1, 1, 10, 10);

    ass->textures[TEX_WALL_1] = LoadTexture("resources/wall_1.png");
    ass->textures[TEX_WALL_2] = LoadTexture("resources/wall_2.png");
    ass->textures[TEX_WALL_3] = LoadTexture("resources/wall_3.png");
    ass->textures[TEX_PROPS] = LoadTexture("resources/props.png");
    ass->textures[TEX_CHAINLINK_FENCE] = LoadTexture("resources/chainlink.png");
    ass->textures[TEX_FLOOR_1] = LoadTexture("resources/floor_1.png");
    ass->textures[TEX_CHAR_1] = LoadTexture("resources/char.png");
    ass->textures[TEX_SALAMI] = LoadTexture("resources/salami.png");
    ass->textures[TEX_PINEAPPLE] = LoadTexture("resources/pineapple.png");
    ass->textures[TEX_ORANGE] = LoadTexture("resources/orange.png");
    ass->textures[TEX_GIRL_1] = LoadTexture("resources/girls/girl_1.png");
    ass->textures[TEX_GIRL_2] = LoadTexture("resources/girls/girl_2.png");
    ass->textures[TEX_GIRL_3] = LoadTexture("resources/girls/girl_3.png");
    ass->textures[TEX_GIRL_4] = LoadTexture("resources/girls/girl_4.png");
    ass->textures[TEX_GRASS_1] = LoadTexture("resources/textures/grass_1.png");

    ass->shaders[SHADER_PHONG_LIGHTING] =
        hotload_shader(ass, "resources/phong_vs.glsl",
                       "resources/phong_fs.glsl", SHADER_PHONG_LIGHTING);

    ass->shaders[SHADER_SKYBOX] =
        LoadShader("resources/skybox_vs.glsl", "resources/skybox_fs.glsl");
    ass->shaders[SHADER_CUBEMAP] =
        LoadShader("resources/cubemap_vs.glsl", "resources/cubemap_fs.glsl");

    ass->fonts[FONT_MAIN_FONT] = LoadFont("resources/alagard_font.png");

    Shader* shader = &ass->shaders[SHADER_PHONG_LIGHTING];

    shader->locs[LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");
    shader->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");

    ass->sun = CreateSun(*shader, SUN_DIRECTION, (Color){20, 20, 20, 255},
                         (Color){200, 200, 200, 255});

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

void assets_load_scripts(Assets* self, JanetTable* env) {
    Janet result;
    janet_dostring(env, LoadText("resources/scripts/simple_zombie_ai.janet"),
                   "main", &result);

    JanetType type = janet_type(result);

    if (janet_checktype(result, JANET_FUNCTION)) {
        JanetFunction* func = janet_unwrap_function(result);
        janet_gclock(func);
        self->scripts[SCRIPTS_BASIC_ZOMBIE_AI] = func;
    } else {
        printf("Not a function\n");
    }
}

void update_assets(Assets* self) {
    for (int id = 0; id < SHADER_NUM_SHADERS; id++) {
        struct Hotload* hot = &self->shaders_hotload[id];

        if (hot->hotload == false) continue;

        hot->just_hotloaded = false;

        long times[] = {GetFileModTime(hot->paths[SHADER_VERTEX]),
                        GetFileModTime(hot->paths[SHADER_FRAGMENT])};

        for (int j = 0; j < 2; j++) {
            // printf("times fs %d != %d\n", times[1],
            // hot->last_time_modified[1]);
            if (times[j] != hot->last_time_modified[j]) {
                printf("Hotloading shader [%s] [%s]\n",
                       hot->paths[SHADER_VERTEX], hot->paths[SHADER_FRAGMENT]);
                UnloadShader(self->shaders[id]);
                self->shaders[id] =
                    hotload_shader(self, hot->paths[SHADER_VERTEX],
                                   hot->paths[SHADER_FRAGMENT], id);

                hot->just_hotloaded = true;
            }
        }
    }
}