#include "behaviours.h"

Actor create_actor(int type) { return (Actor){.type = type}; }

void update_end_target(EcsWorld* ecs, EntStruct* self, Actor* actor) {
    float dt = (float)GetFrameTime();
    float time = (float)GetTime();

    get_comp(ecs, self, Transform)->rotation =
        QuaternionFromEuler(time * 1.1f, time * 1.2f, time * 1.3f);
}

void update_behaviours(EcsWorld* ecs, EntId id) {
    EntStruct* self = get_ent(ecs, id);

    if (!has_comp(ecs, self, Actor)) {
        return;
    }

    Actor* actor = get_comp(ecs, self, Actor);

    switch (actor->type) {
        case END_TARGET: {
            update_end_target(ecs, self, actor);
            break;
        }
        default:
            return;
    }
}