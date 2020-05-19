#include "components.h"

#define DOOR_INTERACTION_DISTANCE (5)

int contains(Body* a, Body* b) {
    return a->pos.x + a->size.x > b->pos.x && a->pos.x < b->pos.x + b->size.x &&
           a->pos.y + a->size.y > b->pos.y && a->pos.y < b->pos.y + b->size.y;
}

V2 center(Body* b) {
    return (V2){
        .x = b->pos.x + b->size.x * 0.5,
        .y = b->pos.y + b->size.y * 0.5,
    };
}

void update_timed_destroy(EcsWorld* world, EntId id) {
    EntStruct* e = get_ent(world, id);
    if (!has_comp(world, e, TimedDestroy)) return;

    TimedDestroy* td = get_comp(world, e, TimedDestroy);

    if (td->done) {
        kill_ent(e);
        return;
    }

    if (td->time_left <= 0.0f) {
        // We set this flag to allow a single frame where we can
        // run code to clean up or do something on time up
        td->done = true;
        return;
    }

    td->time_left -= GetFrameTime();
}

void update_doors(EcsWorld* ecs, EntId id) {
    EntStruct* self = get_ent(ecs, id);

    if (!has_comp(ecs, self, Door) || !has_comp(ecs, self, Model)) {
        return;
    }

    Door* door = get_comp(ecs, self, Door);
    Model* model = get_comp(ecs, self, Model);
    Transform* transform = get_comp(ecs, self, Transform);

    EntId player_id = get_first_with(ecs, Player);
    if (player_id < 0) return;

    EntStruct* player = get_ent(ecs, player_id);

    Transform* player_transform = get_comp(ecs, player, Transform);

    // Get distance from player
    float distance =
        Vector3Distance(player_transform->translation, transform->translation);

    if (distance < DOOR_INTERACTION_DISTANCE && door->state == Door_Closed &&
        IsKeyPressed(KEY_SPACE)) {
        if (door->facing == Facing_X) {
            get_comp(ecs, self, Physics)->velocity.x = 600 * GetFrameTime();
        }

        if (door->facing == Facing_Z) {
            get_comp(ecs, self, Physics)->velocity.z = 600 * GetFrameTime();
        }

        door->state = Door_Open;
    }
}
