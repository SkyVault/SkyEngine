#include "assembler.h"

EntId PLAYER_C(Game* game, float x, float y, float vx, float vy) {
    EntId player_id = create_ent(game->ecs);
    EntStruct* player = get_ent(game->ecs, player_id);

    add_comp(game->ecs, player, Transform,
             .translation = (Vector3){x, 1.0f, y});
    add_comp(game->ecs, player, Player, .n = 0);
    add_comp_obj(game->ecs, player, Physics, create_physics());
    return player_id;
}

EntId PINEAPPLE_BOMB_C(Game* game, float x, float y, float vx, float vy) {
    EntId bullet_id = create_ent(game->ecs);
    EntStruct* bullet = get_ent(game->ecs, bullet_id);

    return bullet_id;
}

EntId GIRL_1_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_1],
             .material = (Material){0}, .scale = 6);

    return self_id;
}

EntId GIRL_2_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_2],
             .material = (Material){0}, .scale = 6);

    return self_id;
}

EntId GIRL_3_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_3],
             .material = (Material){0}, .scale = 6);

    return self_id;
}

EntId GIRL_4_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0, y},
             .rotation = QuaternionIdentity());

    add_comp(game->ecs, self, Billboard,
             .texture = game->assets->textures[TEX_GIRL_4],
             .material = (Material){0}, .scale = 6);

    return self_id;
}

EntId END_TARGET_C(Game* game, float x, float y, float vx, float vy) {
    EntId self_id = create_ent(game->ecs);
    EntStruct* self = get_ent(game->ecs, self_id);

    add_comp(game->ecs, self, Transform, .translation = (Vector3){x, 0.0f, y});
    add_comp(game->ecs, self, Actor, .type = END_TARGET);

    add_comp_obj(game->ecs, self, Model,
                 LoadModelFromMesh(game->assets->meshes[MESH_CUBE]));

    Model* model = get_comp(game->ecs, self, Model);
    Transform* trans = get_comp(game->ecs, self, Transform);

    model->materials[0].maps[MAP_DIFFUSE].color = (Color){0, 255, 255, 100};
    model->materials[0].shader = game->assets->shaders[SHADER_PHONG_LIGHTING];

    trans->rotation = QuaternionFromAxisAngle((Vector3){0, 1, 0}, 45.f);

    return self_id;
}

EntId NUM_ENTITY_TYPES_C(Game* game, float x, float y, float vx, float vy) {
    return -1;
}

EntId assemble(int which, Game* game, float x, float y, float vx, float vy) {
    return assemblers[which](game, x, y, vx, vy);
}