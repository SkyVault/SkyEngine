#include "physics.h"

#define GRAVITY (10.0f)
#define GROUND (-1.6f)

void update_physics(Map* map, EcsWorld* ecs, Game* game, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);
    if (!has_comp(ecs, self, Physics) || !has_comp(ecs, self, Transform)) {
        return;
    }

    const float dt = GetFrameTime();

    Physics* physics = get_comp(ecs, self, Physics);
    Transform* trans = get_comp(ecs, self, Transform);

    // Collision checking
    float tot_h = map->height * CUBE_SIZE;
    float tot_w = map->width * CUBE_SIZE;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            bool active = map->walls[0][x + y * map->width].active;

            if (game->noclip) continue;

            if (active) {
                Rectangle wall = (Rectangle){x * CUBE_SIZE, y * CUBE_SIZE,
                                             CUBE_SIZE, CUBE_SIZE};
                Rectangle body = (Rectangle){trans->translation.x - 0.5f,
                                             trans->translation.z - 0.5f,
                                             CUBE_SIZE, CUBE_SIZE};

                Vector2 pos = {trans->translation.x, trans->translation.z};

                if (CheckCollisionCircleRec(
                        (Vector2){body.x + physics->velocity.x * dt, body.y},
                        0.2f, wall)) {
                    physics->velocity.x = 0.f;
                }

                if (CheckCollisionCircleRec(
                        (Vector2){body.x, body.y + physics->velocity.z * dt},
                        0.2f, wall)) {
                    physics->velocity.z = 0;
                }
            }
        }
    }

    trans->translation.x += physics->velocity.x * dt;
    trans->translation.y += physics->velocity.y * dt;
    trans->translation.z += physics->velocity.z * dt;

    if (trans->translation.y <= GROUND) {
        trans->translation.y = GROUND;
        physics->velocity.y = 0.0f;
        physics->grounded = true;
    } else {
        trans->translation.y -= physics->gravity_scale * GRAVITY * dt;
    }

    if (physics->grounded && physics->bounce_factor > 0.1f &&
        trans->translation.y <= GROUND) {
        physics->velocity.y = GRAVITY * physics->bounce_factor * 2.0f;
        physics->bounce_factor *= powf(0.001f, GetFrameTime());
    }

    if (physics->velocity.y <= 0.15) {
        physics->velocity.y = 0.f;
    }

    if (physics->bounce_factor <= 0.4f) {
        physics->bounce_factor = 0.f;
    }

    if (physics->friction != 1.f) {
        physics->velocity.x *= powf(physics->friction, GetFrameTime());
        physics->velocity.y *= powf(0.2f, GetFrameTime());
        physics->velocity.z *= powf(physics->friction, GetFrameTime());
    }
}
