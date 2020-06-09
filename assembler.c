#include "assembler.h"

const char* code = "(defn update [] (print \"hello\"))";

EntId ACTOR_PLAYER_C(Game* game, float x, float y, float z, float vx,
                     float vy) {
    EntId player_id = create_ent(game->ecs);
    EntStruct* player = get_ent(game->ecs, player_id);

    add_comp(game->ecs, player, Transform, .translation = (Vector3){x, y, z});
    add_comp(game->ecs, player, Player, .n = 0);
    add_comp_obj(game->ecs, player, Physics, create_physics());

    return player_id;
}

EntId ACTOR_PINEAPPLE_BOMB_C(Game* game, float x, float y, float z, float vx,
                             float vy) {
    EntId bullet_id = create_ent(game->ecs);
    EntStruct* bullet = get_ent(game->ecs, bullet_id);

    return bullet_id;
}

EntId ACTOR_TOSSED_ORANGE_C(Game* game, float x, float y, float z, float vx,
                            float vy) {
    return -1;
}

EntId ACTOR_GIRL_1_C(Game* game, float x, float y, float z, float vx,
                     float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, z},
             .rotation = QuaternionIdentity(), .scale = Vector3One());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_1],
             .material = (Material){0}, .scale = GLOBAL_SCALE);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_1));

    add_comp(game->ecs, self, Script, .which = SCRIPTS_BASIC_ZOMBIE_AI);

    return self_id;
}

EntId ACTOR_GIRL_2_C(Game* game, float x, float y, float z, float vx,
                     float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, z},
             .rotation = QuaternionIdentity(), .scale = Vector3One());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_2],
             .material = (Material){0}, .scale = GLOBAL_SCALE);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_2));

    return self_id;
}

EntId ACTOR_GIRL_3_C(Game* game, float x, float y, float z, float vx,
                     float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, z},
             .rotation = QuaternionIdentity(), .scale = Vector3One());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_3],
             .material = (Material){0}, .scale = GLOBAL_SCALE);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_3));

    return self_id;
}

EntId ACTOR_GIRL_4_C(Game* game, float x, float y, float z, float vx,
                     float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, z},
             .rotation = QuaternionIdentity(), .scale = Vector3One());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_4],
             .material = (Material){0}, .scale = GLOBAL_SCALE);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_4));

    return self_id;
}

EntId ACTOR_END_TARGET_C(Game* game, float x, float y, float z, float vx,
                         float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0.0f, z},
             .rotation = QuaternionIdentity(), .scale = Vector3One());

    add_comp(game->ecs, self, Actor, .type = ACTOR_END_TARGET);

    add_comp_obj(game->ecs, self, Model,
                 LoadModelFromMesh(game->assets->meshes[MESH_CUBE]));

    Model* model = get_comp(game->ecs, self, Model);
    model->materials[0].maps[MAP_DIFFUSE].color = (Color){0, 255, 255, 100};
    model->materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];

    return self_id;
}

EntId ACTOR_BLOCK_C(Game* game, float x, float y, float z, float vx, float vy) {
    EntStruct* self = create_and_get_ent(game->ecs);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0.0f, z},
             .rotation = QuaternionIdentity(),
             .scale = (Vector3){CUBE_SIZE, CUBE_SIZE, CUBE_SIZE});

    add_comp_obj(game->ecs, self, Model,
                 LoadModelFromMesh(game->assets->meshes[MESH_CUBE]));

    add_comp(game->ecs, self, Actor, .type = ACTOR_BLOCK);

    Model* model = get_comp(game->ecs, self, Model);
    model->materials[0].maps[MAP_DIFFUSE].color = (Color){255, 255, 0, 255};
    model->materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];

    return self->id;
}

EntId ACTOR_NUM_ENTITY_TYPES_C(Game* game, float x, float y, float z, float vx,
                               float vy) {
    return -1;
}

EntId assemble(int which, Game* game, float x, float y, float z, float vx,
               float vy) {
    return assemblers[which](game, x, y, z, vx, vy);
}