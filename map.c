#include "map.h"

EntStruct* create_door(Vector3 pos, Game* game) {
    EcsWorld* ecs = game->ecs;
    EntId id = create_ent(ecs);
    EntStruct* self = get_ent(ecs, id);
    add_comp_obj(ecs, self, Physics, create_physics());
    add_comp(ecs, self, Transform, .translation=pos);
    add_comp(ecs, self, Door, .facing=Facing_X, .state=Door_Closed, .timer=0);
    get_comp(ecs, self, Physics)->friction = 0.2f;
    return self;
}

void create_x_door(Vector3 pos, Game* game) {
    EntStruct* self = create_door(pos, game);
    Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    model.transform =
        MatrixMultiply(
            model.transform,
            MatrixScale(CUBE_SIZE, CUBE_SIZE, 0.5f*CUBE_SIZE));
    model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
    add_comp_obj(game->ecs, self, Model, model);
}

void create_z_door(Vector3 pos, Game* game) {
    EntStruct* self = create_door(pos, game);
    Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    model.transform =
        MatrixMultiply(
            model.transform,
            MatrixScale(0.5f*CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));
    model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
    add_comp_obj(game->ecs, self, Model, model);
    get_comp(game->ecs, self, Door)->facing = Facing_Z;
}

Map* load_map(int map, Game* game) {
    Map* result = malloc(sizeof(Map));
    result->current_map = map;

    for (int i = 0; i < MAX_MAP_WIDTH*MAX_MAP_HEIGHT; i++)
        result->walls[i].active = 0;

    const struct LvlData* data = &map_data[result->current_map];

    Model default_wall = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    default_wall.materials[0].maps[MAP_DIFFUSE].texture = game->assets->textures[TEX_WALL_1];
    default_wall.materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];
    
    for (int z = 0; z < data->h; z++) {
        for (int x = 0; x < data->w; x++) {
            char chr = data->d[x + z * data->w];

            // Load the entities and tiles
            Vector3 pos = (Vector3){
                       .x = x * CUBE_SIZE,
                       .y = 0,
                       .z = z * CUBE_SIZE
            };
            switch (chr) {
                case '|':
                    create_z_door(pos, game);
                    break;
                case '-':
                    create_x_door(pos, game);
                    break;
                case '#':
                    break;
                case ' ': break;
            }

            if (chr != ' ') {
                result->walls[x + z * data->w].active = 1;
                result->walls[x + z * data->w].model = default_wall;
            } else {
                result->walls[x + z * data->w].active = 0;
            }
        }
    }

    result->floor_tile_models[0] = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
    result->floor_tile_models[0].materials[0].maps[MAP_DIFFUSE].texture = game->assets->textures[TEX_FLOOR_1];
    result->floor_tile_models[0].materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];

    return result;
}

void update_map(Map* map, Game* game) {

}

void draw_map(Map* map, Game* game) {
    const struct LvlData* data = &map_data[map->current_map];

    for (int z = 0; z < data->h; z++) {
        for (int x = 0; x < data->w; x++) {
            char chr = data->d[x + z * data->w];

            Vector3 pos = (Vector3){x * CUBE_SIZE, 0, z * CUBE_SIZE};

            DrawModel(
                map->floor_tile_models[0],
                (Vector3){x * CUBE_SIZE, -CUBE_SIZE, z * CUBE_SIZE},
                CUBE_SIZE,
                RAYWHITE);

            if (map->walls[x+z*data->w].active) {
                switch (chr) {
                    case '#':
                        DrawModel(
                            map->walls[x + z * data->w].model,
                            pos,
                            CUBE_SIZE,
                            RAYWHITE);
                        break;
                    case ' ': break;
                }
            }
        }
    }
}
