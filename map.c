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

    result->models[0] = default_wall;
    result->models[1] = default_wall_2;

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
    result->num_layers = 0;

    zero_out_map(result);
    load_models(result, game);

    // Read the map size
    JanetArray *size_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("size")));

    JanetArray *layers_arr = janet_unwrap_array(
        janet_table_get(result_table, janet_ckeywordv("layers")));

    assert(size_arr->count == 2);

    result->width = janet_unwrap_integer(size_arr->data[0]);
    result->height = janet_unwrap_integer(size_arr->data[1]);

    result->player_x = 2;
    result->player_y = 2;

    for (int layer = 0; layer < layers_arr->count; layer++) {
        JanetTable *layer_table = janet_unwrap_table(layers_arr->data[layer]);

        JanetString *data = janet_unwrap_string(
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
                    result->player_y = pos.z + CUBE_SIZE / 2;
                    break;
                case 'E': {
                    assemble(ACTOR_END_TARGET, game, pos.x + CUBE_SIZE / 2,
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

Map *load_map_from_file(const char *path, Game *game) {
    FILE *o = fopen(path, "r");

    fseek(o, 0L, SEEK_END);
    size_t len = ftell(o);
    fseek(o, 0L, SEEK_SET);

    Map *result = malloc(sizeof(Map));
    result->current_map = -1;
    result->num_layers = 0;

    result->path.len = strlen(path);
    result->path.buff = malloc(result->path.len + 1);
    for (int i = 0; i < result->path.len; i++) result->path.buff[i] = path[i];
    result->path.buff[result->path.len] = '\0';

    result->player_x = 0.0f;
    result->player_y = 0.0f;

    zero_out_map(result);
    load_models(result, game);

    int w = 0;
    int h = 0;

    bool size_set = false;

    char *line = NULL;
    size_t slen = 0;
    size_t read;

    while ((read = getline(&line, &slen, o)) != -1) {
        if (slen > 0) {
            char fst = line[0];

            switch (fst) {
                case ':': {
                    char name[128];
                    sscanf(line, ":%s", name);

                    if (is_number(name)) {
                        if (!size_set) {
                            printf(
                                "ERROR:: Map data is missing a size "
                                "constant\n");
                            return result;
                        }

                        int layer = atoi(name);

                        for (int y = 0; y < result->height; y++) {
                            read = getline(&line, &slen, o);

                            if (slen == 0) {
                                printf(
                                    "ERROR:: blank line in the data segment\n");
                                return result;
                            }
                            printf("line: %s", line);

                            for (int x = 0; x < result->width; x++) {
                                Vector3 pos =
                                    (Vector3){.x = (float)x * CUBE_SIZE,
                                              .y = 0.f,
                                              .z = (float)y * CUBE_SIZE};

                                switch (line[x]) {
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
                                        result->player_x =
                                            pos.x + CUBE_SIZE / 2;
                                        result->player_y =
                                            pos.z + CUBE_SIZE / 2;
                                        break;
                                    case 'E': {
                                        assemble(ACTOR_END_TARGET, game,
                                                 pos.x + CUBE_SIZE / 2,
                                                 pos.z + CUBE_SIZE / 2, 0, 0);
                                        break;
                                    }

                                    default:
                                        result
                                            ->walls[layer]
                                                   [x + y * result->width]
                                            .active = 1;
                                        const char buff[] = {line[x], '\0'};
                                        int i = atoi(buff) - 1;
                                        result
                                            ->walls[layer]
                                                   [x + y * result->width]
                                            .model = i;
                                }
                            }
                        }
                    } else if (strcmp(name, "entities")) {
                        // Parse the entities layer
                    }
                }

                default: {
                    char name[128];
                    char type[128];

                    sscanf(line, "%s %s", name, type);

                    if (strcmp(type, "i") == 0) {
                        int i;
                        sscanf(line, "%s %s %d", name, type, &i);
                        break;
                    } else if (strcmp(type, "2i") == 0) {
                        int x, y;
                        sscanf(line, "%s %s %d %d", name, type, &x, &y);

                        if (strcmp(name, "size") == 0) {
                            result->width = x;
                            result->height = y;
                            size_set = true;
                        } else {
                            // printf("Unknown constant %s\n", name);
                        }

                        break;
                    } else if (strcmp(type, "str") == 0) {
                    }
                }
            }
        }
    }

    fclose(o);
    o = NULL;

    return result;
}

void update_map(Map *map, Game *game) {}

void draw_map(Map *map, Game *game) {
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
                              RAYWHITE);
                }

                if (map->walls[layer][x + z * map->width].active) {
                    DrawModel(map->models[map->walls[layer][x + z * map->width]
                                              .model],
                              pos, CUBE_SIZE, RAYWHITE);
                }
            }
        }
    }
}

void destroy_map(Map *map, Game *game) {
    for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
        for (int y = 0; y < MAX_MAP_HEIGHT; y++) {
            for (int x = 0; x < MAX_MAP_WIDTH; x++) {
                map->walls[layer][x + y * MAX_MAP_WIDTH] = (Wall){0, 0};
            }
        }
    }

    for (int mi = 0; mi < MAX_MODELS; mi++) {
        map->models[mi] = (Model){0};
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
    load_map_from_file(s, game);
}