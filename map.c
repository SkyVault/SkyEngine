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

Map *load_map_from_file(const char *path, Game *game) {
    FILE *o = fopen(path, "r");

    fseek(o, SEEK_END, 0);
    size_t len = ftell(o);
    fseek(o, SEEK_SET, 0);

    Map *result = malloc(sizeof(Map));
    result->current_map = -1;
    result->num_layers = 0;

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

                                    case 'E': {
                                        assemble(ACTOR_END_TARGET, game, pos.x,
                                                 pos.z, 0, 0);
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
                    }
                }
            }
        }
    }

    return result;
}

Map *load_map(int map, Game *game) {
    Map *result = malloc(sizeof(Map));
    result->current_map = map;

    zero_out_map(result);
    load_models(result, game);

    const struct LvlData *data = &map_data[result->current_map];

    result->width = data->w;
    result->height = data->h;

    for (int z = 0; z < data->h; z++) {
        for (int x = 0; x < data->w; x++) {
            char chr = data->d[x + z * data->w];

            // Load the entities and tiles
            Vector3 pos = (Vector3){
                .x = (float)x * CUBE_SIZE, .y = 0.f, .z = (float)z * CUBE_SIZE};

            switch (chr) {
                case '|':
                    create_z_door(pos, game);
                    break;
                case '-':
                    create_x_door(pos, game);
                    break;
                case '#':
                    break;
                case ' ':
                    break;
            }

            if (chr == '#') {
                result->walls[0][x + z * data->w].active = 1;
                result->walls[0][x + z * data->w].model = 0;
            } else {
                result->walls[0][x + z * data->w].active = 0;
                result->walls[0][x + z * data->w].model = 0;
            }
        }
    }

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
