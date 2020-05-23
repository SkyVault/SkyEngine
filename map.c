#include "map.h"

#include "rlgl.h"
#include "utils.h"

EntStruct *create_door(Vector3 pos, Game *game) {
    EcsWorld *ecs = game->ecs;
    EntId id = create_ent(ecs);
    EntStruct *self = get_ent(ecs, id);
    add_comp_obj(ecs, self, Physics, create_physics());
    add_comp(ecs, self, Transform, .translation = pos,
             .rotation = QuaternionIdentity());
    add_comp(ecs, self, Door, .facing = Facing_X, .state = Door_Closed,
             .timer = 0);
    get_comp(ecs, self, Physics)->friction = 0.2f;
    return self;
}

void create_x_door(Vector3 pos, Game *game) {
    EntStruct *self = create_door(pos, game);
    Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    model.transform = MatrixMultiply(
        model.transform, MatrixScale(CUBE_SIZE, CUBE_SIZE, 0.5f * CUBE_SIZE));
    model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
    add_comp_obj(game->ecs, self, Model, model);
}

void create_z_door(Vector3 pos, Game *game) {
    EntStruct *self = create_door(pos, game);
    Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    model.transform = MatrixMultiply(
        model.transform, MatrixScale(0.5f * CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));
    model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
    add_comp_obj(game->ecs, self, Model, model);
    get_comp(game->ecs, self, Door)->facing = Facing_Z;
}

bool is_number(const char *str) {
    size_t len = strlen(str);
    if (len == 0) return false;
    bool point = false;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '-' && i != 0) return false;
        if (str[i] == '.') {
            if (point) {
                return false;
            } else
                point = true;
            continue;
        }

        if (isdigit(str[i]) == false) return false;
    }
    return true;
}

void zero_out_map(Map *result) {
    for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
        for (int i = 0; i < MAX_MAP_WIDTH * MAX_MAP_HEIGHT; i++) {
            if (layer == 0) {
                result->walls[layer][i].active = 0;
                result->walls[layer][i].model = 0;
            } else {
                result->walls[layer][i].active = 0;
                result->walls[layer][i].model = 0;
            }
        }
    }
}

void load_models(Map *result, Game *game) {
    Model default_wall = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    default_wall.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_WALL_1];
    default_wall.materials[0].shader =
        game->assets->shaders[SHADER_PHONG_LIGHTING];

    Model default_wall_2 = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    default_wall_2.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_WALL_2];
    default_wall_2.materials[0].shader =
        game->assets->shaders[SHADER_PHONG_LIGHTING];

    Model default_wall_3 = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    default_wall_2.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_WALL_3];
    default_wall_2.materials[0].shader =
        game->assets->shaders[SHADER_PHONG_LIGHTING];

    Model fence_1 = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    fence_1.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_CHAINLINK_FENCE];
    fence_1.materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];
    fence_1.transform = MatrixIdentity();
    fence_1.transform =
        MatrixMultiply(fence_1.transform, MatrixScale(0.01f, 1.0f, 1.0f));

    Model fence_2 = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    fence_2.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_CHAINLINK_FENCE];
    fence_2.materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];
    fence_2.transform = MatrixIdentity();
    fence_2.transform =
        MatrixMultiply(fence_2.transform, MatrixScale(1.0f, 1.0f, 0.01f));

    result->models[0] = default_wall;
    result->models[1] = default_wall_2;
    result->models[3] = default_wall_3;

    result->models[2] = fence_1;
    result->models[3] = fence_2;
    result->num_models = 5;

    result->floor_tile_models[0] =
        LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    result->floor_tile_models[0].materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_FLOOR_1];
    result->floor_tile_models[0].materials[0].shader =
        game->assets->shaders[SHADER_PHONG_LIGHTING];
}

Map *load_map_from_script(const char *path, Game *game) {
    FILE *o = fopen(path, "r");

    fseek(o, 0L, SEEK_END);
    size_t len = ftell(o);
    fseek(o, 0L, SEEK_SET);

    tstr s = talloc(len);
    fread(s, sizeof(char), len, o);

    Janet resultj;
    int res = janet_dostring(game->env, s, "main", &resultj);
    JanetTable *result_table = janet_unwrap_table(resultj); 

    Map *result = malloc(sizeof(Map));
    result->current_map = -1;

    if (result_table == NULL) return result;

    zero_out_map(result);
    load_models(result, game);

    // Read the map size
    JanetArray *size_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("size")));

    JanetArray *start_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("start")));

    JanetArray *layers_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("layers")));

    JanetArray *props_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("props")));

    JanetArray *lights_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("lights")));

    JanetArray *actors_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("actors")));

    assert(size_arr->count == 2);

    result->width = janet_unwrap_integer(size_arr->data[0]);
    result->height = janet_unwrap_integer(size_arr->data[1]);

    result->player_x = janet_unwrap_number(start_arr->data[0]);
    result->player_z = janet_unwrap_number(start_arr->data[1]);

    result->num_layers = MAX_NUM_LAYERS;
    result->num_props = 0;

    Shader *shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

    memset(result->light_color, 0, sizeof(int) * MAX_LIGHTS);
    game->num_lights = 0;
    result->num_spawns = 0;

    if (props_arr != NULL) {
        for (int prop = 0; prop < props_arr->count; prop++) {
            JanetArray *prop_arr = janet_unwrap_array(props_arr->data[prop]);

            float rx = (float)janet_unwrap_number(prop_arr->data[0]);
            float ry = (float)janet_unwrap_number(prop_arr->data[1]);
            float rw = (float)janet_unwrap_number(prop_arr->data[2]);
            float rh = (float)janet_unwrap_number(prop_arr->data[3]);
            float x = (float)janet_unwrap_number(prop_arr->data[4]);
            float y = (float)janet_unwrap_number(prop_arr->data[5]);
            float z = (float)janet_unwrap_number(prop_arr->data[6]);
            float s = (float)janet_unwrap_number(prop_arr->data[7]);

            result->props[result->num_props++] = (Prop){
                .region = (Rectangle){rx, ry, rw, rh},
                .position = (Vector3){x, y, z},
                .scale = s,
            };

            printf("props: %d\n", result->num_props);
        }
    }

    if (lights_arr != NULL) {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            game->lights[i].enabled = false;
            result->light_color[i] = 0;

            if (i < lights_arr->count) {
                JanetArray *light_arr = janet_unwrap_array(lights_arr->data[i]);

                bool enabled = (bool)janet_unwrap_boolean(light_arr->data[0]);
                float x = (float)janet_unwrap_number(light_arr->data[1]);
                float y = (float)janet_unwrap_number(light_arr->data[2]);
                float z = (float)janet_unwrap_number(light_arr->data[3]);
                int light_index = (int)janet_unwrap_integer(light_arr->data[4]);

                result->light_color[i] = light_index;

                game->lights[i].position = (Vector3){x, y, z};
                game->lights[i].color = LightColors[result->light_color[i]];
                game->lights[i].enabled = true;
                game->num_lights++;
            }

            UpdateLightValues(*shader, game->lights[i]);
        }

    } else {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            UpdateLightValues(*shader, game->lights[i]);
        }
    }

    if (actors_arr != NULL) {
        for (int actor_i = 0; actor_i < actors_arr->count; actor_i++) {
            JanetArray *actor_arr =
                janet_unwrap_array(actors_arr->data[actor_i]);

            int type = (int)janet_unwrap_integer(actor_arr->data[0]);
            float x = (float)janet_unwrap_number(actor_arr->data[1]);
            float y = (float)janet_unwrap_number(actor_arr->data[2]);
            float z = (float)janet_unwrap_number(actor_arr->data[3]);
            result->spawns[result->num_spawns++] = (ActorSpawn){
                .type = ACTOR_GIRL_1 + type,
                .position = (Vector3){x, y, z},
            };
            assemble(ACTOR_GIRL_1 + type, game, x, y, z, 0, 0);
        }
    }

    for (int layer = 0; layer < layers_arr->count; layer++) {
        JanetTable *layer_table = janet_unwrap_table(layers_arr->data[layer]);

        const JanetString *data = janet_unwrap_string(
            janet_table_get(layer_table, janet_ckeywordv("data")));

        char *it = data;
        char *end = it + strlen(data);
        while (it != end && it != NULL && isspace(*it)) ++it;

        int x = 0;
        int y = 0;
        while (it != end) {
            if (*it == '\n') {
                ++it;
                y++;
                x = 0;
                continue;
            }

            if (y >= result->height) break;

            char ch = *it;

            Vector3 pos = (Vector3){
                .x = (float)x * CUBE_SIZE, .y = 0.f, .z = (float)y * CUBE_SIZE};

            switch (ch) {
                case '|':
                    create_z_door(pos, game);
                    break;
                case '-':
                    create_x_door(pos, game);
                    break;
                case ' ':
                case '.':
                    break;

                case 'X':
                    result->player_x = pos.x + CUBE_SIZE / 2;
                    result->player_z = pos.z + CUBE_SIZE / 2;
                    break;
                case 'E': {
                    assemble(ACTOR_END_TARGET, game, pos.x + CUBE_SIZE / 2, 0,
                             pos.z + CUBE_SIZE / 2, 0, 0);
                    break;
                }
                default: {
                    char buff[] = {ch, '\0'};
                    int i = atoi(buff) - 1;
                    result->walls[layer][x + y * result->width].model = i;
                    result->walls[layer][x + y * result->width].active = 1;
                    break;
                }
            }

            ++x;
            ++it;
        }
    }

    fclose(o);

    return result;
}

void update_map(Map *self, Game *game) {}

void render_map(Map *map, GfxState *gfx, Game *game) {
    rlEnableBackfaceCulling();
    rlEnableDepthTest();
    for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
        for (int z = 0; z < map->height; z++) {
            for (int x = 0; x < map->width; x++) {
                Vector3 pos =
                    (Vector3){x * CUBE_SIZE + CUBE_SIZE / 2, layer * CUBE_SIZE,
                              z * CUBE_SIZE + CUBE_SIZE / 2};

                if (layer == 0) {
                    DrawModel(map->floor_tile_models[0],
                              (Vector3){pos.x, -CUBE_SIZE, pos.z}, CUBE_SIZE,
                              WHITE);
                }

                if (map->walls[layer][x + z * map->width].active ||
                    map->walls[layer][x + z * map->width].model > 0) {
                    DrawModel(map->models[map->walls[layer][x + z * map->width]
                                              .model],
                              pos, CUBE_SIZE, WHITE);
                }
            }
        }
    }

    // billboards
    for (int pi = 0; pi < map->num_props; pi++) {
        draw_prop(gfx, game, map->props[pi]);

        // Prop prop = map->props[pi];
        // DrawBillboardRec(*game->camera, game->assets->textures[TEX_PROPS],
        //                  prop.region, prop.position, prop.scale, WHITE);
    }
}

void destroy_map(Map *map, Game *game) {
    for (int mi = 0; mi < MAX_MODELS; mi++) {
        UnloadModel(map->models[mi]);
    }

    if (map->path.len > 0 && map->path.buff != NULL) {
        free(map->path.buff);
        map->path.buff = NULL;
        map->path.len = -1;
    }
}

void reload_map(Map *map, Game *game) {
    tstr s = tstrf("%s", map->path);
    destroy_map(map, game);
}