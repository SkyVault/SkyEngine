#include "physics.h"

#define GRAVITY (10.0f)
#define GROUND (-1.6f)

void update_physics(Region* map, EcsWorld* ecs, Game* game, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);
    if (!has_comp(ecs, self, Physics) || !has_comp(ecs, self, Transform)) {
        return;
    }

    const float dt = GetFrameTime();

    Physics* physics = get_comp(ecs, self, Physics);
    Transform* trans = get_comp(ecs, self, Transform);

    // Collision checking


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
