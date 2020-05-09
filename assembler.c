#include "assembler.h"

EntId ACTOR_PLAYER_C(Game* game, float x, float y, float vx, float vy) {
    EntId player_id = create_ent(game->ecs);
    EntStruct* player = get_ent(game->ecs, player_id);

    add_comp(game->ecs, player, Transform,
             .translation = (Vector3){x, 1.0f, y});
    add_comp(game->ecs, player, Player, .n = 0);
    add_comp_obj(game->ecs, player, Physics, create_physics());
    return player_id;
}

EntId ACTOR_PINEAPPLE_BOMB_C(Game* game, float x, float y, float vx, float vy) {
    EntId bullet_id = create_ent(game->ecs);
    EntStruct* bullet = get_ent(game->ecs, bullet_id);

    return bullet_id;
}

EntId ACTOR_TOSSED_ORANGE_C(Game* game, float x, float y, float vx, float vy) {
    return -1;
}

EntId ACTOR_GIRL_1_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_1],
             .material = (Material){0}, .scale = 6);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_1));

    return self_id;
}

EntId ACTOR_GIRL_2_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_2],
             .material = (Material){0}, .scale = 6);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_2));

    return self_id;
}

EntId ACTOR_GIRL_3_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_3],
             .material = (Material){0}, .scale = 6);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_3));

    return self_id;
}

EntId ACTOR_GIRL_4_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_4],
             .material = (Material){0}, .scale = 6);

    add_comp_obj(game->ecs, self, Actor, create_enemy_actor(ACTOR_GIRL_4));

    return self_id;
}

EntId ACTOR_END_TARGET_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0.0f, y},
             .rotation = QuaternionIdentity());
    add_comp(game->ecs, self, Actor, .type = ACTOR_END_TARGET);

    add_comp_obj(game->ecs, self, Model,
                 LoadModelFromMesh(game->assets->meshes[MESH_CUBE]));

    Model* model = get_comp(game->ecs, self, Model);
    model->materials[0].maps[MAP_DIFFUSE].color = (Color){0, 255, 255, 100};
    model->materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];

    return self_id;
}

EntId ACTOR_NUM_ENTITY_TYPES_C(Game* game, float x, float y, float vx,
                               float vy) {
    return -1;
}

EntId assemble(int which, Game* game, float x, float y, float vx, float vy) {
    return assemblers[which](game, x, y, vx, vy);
}