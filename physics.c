#include "physics.h"

#define GRAVITY (340.0f)
#define GROUND (-1.6f)
#define MAX_GROUND_ANGLE (120)

void update_physics(Region* map, EcsWorld* ecs, Game* game, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);
    if (!has_comp(ecs, self, Physics) || !has_comp(ecs, self, Transform)) {
        return;
    }

    const float dt = GetFrameTime();

    Physics* physics = get_comp(ecs, self, Physics);
    Transform* trans = get_comp(ecs, self, Transform);

    // Apply gravity
    // if (IsKeyDown(KEY_P))

    if (!physics->grounded) {
        physics->velocity.y -= GRAVITY * physics->gravity_scale * dt;
    }

    // Collision checking

    NodeRayInfo *hit_info_list = NULL;
    if (has_comp(ecs, self, Model)) {
        BoundingBox box = MeshBoundingBox(get_comp(ecs, self, Model)->meshes[0]); 

        const float height = (box.max.y - box.min.y);

        box = transform_bounding_box(box, *trans);

        // TODO(Dustin): we need to account for the @origin
        Ray down_ray = {
            .position = (Vector3){
                trans->translation.x,
                trans->translation.y + height * 0.5f,
                trans->translation.z,
            },
            .direction = (Vector3) {0, -1, 0}
        }; 

        //DrawBoundingBox(box, RED);
        //DrawRay(down_ray, RED);

        hit_info_list = do_raycast(map, down_ray);

        //DrawSphere(hit_info.info.position, 0.25f, (Color){255, 0, 0, 100});
        //DrawSphere(trans->translation, 0.25f, (Color){0, 0, 255, 100});

        if (hit_info_list) {
            hit_info_list = hit_info_list->next;
        }

        while (hit_info_list) {
            if (hit_info_list->info.hit) {
                NodeRayInfo hit_info = *hit_info_list;
                float dist = Vector3Distance(hit_info.info.position, down_ray.position);

                if (dist <= ((height) + 0.1f)) {
                    physics->velocity.y = 0.0f;
                    trans->translation.y = hit_info.info.position.y + height * 0.5f;
                    physics->grounded = true;
                } else {
                    physics->grounded = false;
                }
            }

            hit_info_list = hit_info_list->next;
        }
    }

    // Calculate forward vector
    Vector3 forward = Vector3Normalize(physics->velocity);
    if (physics->grounded) {
        //forward = Vector3CrossProduct(Vector3 v1, Vector3 v2);
            
    }

    if (physics->ground_angle < MAX_GROUND_ANGLE) {
        // Move
        trans->translation.x += physics->velocity.x * dt;
        trans->translation.y += physics->velocity.y * dt;
        trans->translation.z += physics->velocity.z * dt;
    }

    if (physics->velocity.y <= 0.15) {
        physics->velocity.y = 0.f;
    }

    if (physics->friction != 1.f) {
        physics->velocity.x *= powf(physics->friction, GetFrameTime());
        physics->velocity.y *= powf(0.2f, GetFrameTime());
        physics->velocity.z *= powf(physics->friction, GetFrameTime());
    }
}
