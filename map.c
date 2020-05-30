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

void zero_out_map(Map *self) {
    self->num_models = 0;
    self->num_props = 0;
    self->num_spawns = 0;
    self->num_exits = 0;

    for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
        for (int i = 0; i < MAX_MAP_WIDTH * MAX_MAP_HEIGHT; i++) {
            self->walls[layer][i].active = 0;
            self->walls[layer][i].model = 0;
        }
    }
}

Map *create_map_from_script(const char *path, Game *game) {
    Map *result = malloc(sizeof(Map));
    result->current_map = -1;

    size_t psize = strlen(path);
    result->path.buff = malloc(sizeof(char) * psize + 1);
    for (int i = 0; i < psize; i++) result->path.buff[i] = path[i];

    result->path.len = psize;

    zero_out_map(result);

    // Load the models and meshes
    for (int i = 0; i <= 4; i++) {
        result->models[result->num_models++] = game->assets->models[i];
    }

    result->floor_tile_models[0] = game->assets->models[5];

    load_map_from_script(result, path, game);

    return result;
}

void load_map_from_script(Map *result, const char *path, Game *game) {
    FILE *o = fopen(path, "r");

    fseek(o, 0L, SEEK_END);
    size_t len = ftell(o);
    fseek(o, 0L, SEEK_SET);

    tstr s = talloc(len);
    fread(s, sizeof(char), len, o);

    Janet resultj;
    int res = janet_dostring(game->env, s, "main", &resultj);
    JanetTable *result_table = janet_unwrap_table(resultj);

    if (result_table == NULL) return;

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

    if (size_arr) {
        result->width = janet_unwrap_integer(size_arr->data[0]);
        result->height = janet_unwrap_integer(size_arr->data[1]);
    } else {
        printf("Map(%s)::load Missing size, going with the default (%d %d)",
               path, MAX_MAP_WIDTH, MAX_MAP_HEIGHT);
        result->width = MAX_MAP_WIDTH;
        result->height = MAX_MAP_HEIGHT;
    }

    if (start_arr) {
        result->player_x = janet_unwrap_number(start_arr->data[0]);
        result->player_z = janet_unwrap_number(start_arr->data[1]);
    } else {
        printf("Map(%s)::load Missing start, going with default (5 5)");
        result->player_x = 5;
        result->player_z = 5;
    }

    result->num_layers = MAX_NUM_LAYERS;
    result->num_props = 0;

    result->num_exits = 0;

    Shader *shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

    memset(result->light_color, 0, sizeof(int) * MAX_LIGHTS);
    game->assets->num_lights = 0;
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
        }
    }

    if (lights_arr != NULL) {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            game->assets->lights[i].enabled = false;
            result->light_color[i] = 0;

            if (i < lights_arr->count) {
                JanetArray *light_arr = janet_unwrap_array(lights_arr->data[i]);

                bool enabled = (bool)janet_unwrap_boolean(light_arr->data[0]);
                float x = (float)janet_unwrap_number(light_arr->data[1]);
                float y = (float)janet_unwrap_number(light_arr->data[2]);
                float z = (float)janet_unwrap_number(light_arr->data[3]);
                int light_index = (int)janet_unwrap_integer(light_arr->data[4]);

                result->light_color[i] = light_index;

                game->assets->lights[i].position = (Vector3){x, y, z};
                game->assets->lights[i].color =
                    LightColors[result->light_color[i]];
                game->assets->lights[i].enabled = true;
                game->assets->num_lights++;
            }

            UpdateLightValues(*shader, game->assets->lights[i]);
        }

    } else {
        for (int i = 0; i < MAX_LIGHTS; i++) {
            UpdateLightValues(*shader, game->assets->lights[i]);
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

    // Rendering all of the props
    for (int pi = 0; pi < map->num_props; pi++) {
        draw_prop(gfx, game, map->props[pi]);
    }
}

void destroy_map(Map *map, Game *game) {
    zero_out_map(map);

    if (map->path.len > 0 && map->path.buff != NULL) {
        free(map->path.buff);
        map->path.buff = NULL;
        map->path.len = -1;
    }
}

void reload_map(Map *map, Game *game) {
    tstr s = tstrf("%s", map->path);
    destroy_map(map, game);
    load_map_from_script(map, s, game);
}

void add_exit(Map *map, Vector3 position, int id, int dest_id,
              const char *dest_path) {
    Exit result = (Exit){
        .id = id, .dest_id = dest_id, .dest_path = NULL, .position = position};

    size_t len = strlen(dest_path);
    result.dest_path = malloc(len + 1);
    result.dest_path[len] = '\0';
    for (size_t i = 0; i < len; i++) result.dest_path[i] = dest_path[i];

    map->exits[map->num_exits++] = result;
}