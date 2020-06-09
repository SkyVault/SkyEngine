#include "behaviours.h"

Actor create_actor(int type) {
    return (Actor){.type = type, .health = 3.0f, .flags = 0};
}

Actor create_enemy_actor(int type) {
    return (Actor){.type = type, .health = 3.0f, .flags = AF_ENEMY};
}

void update_end_target(EcsWorld* ecs, EntStruct* self, Actor* actor) {
    float dt = (float)GetFrameTime();
    float time = (float)GetTime();

    get_comp(ecs, self, Transform)->rotation =
        QuaternionFromEuler(time * 1.1f, time * 1.2f, time * 1.3f);
}

void update_pinapple_bomb(EcsWorld* ecs, EntStruct* self, Actor* actor) {
    TimedDestroy* td = get_comp(ecs, self, TimedDestroy);

    if (td->done) {
        // Explode
    }
}

void update_enemy(EcsWorld* ecs, Actor* actor, EntId id) {
    EntId proj_id = get_first_with(ecs, PlayerHit);
    if (proj_id < 0) return;

    EntStruct* self = get_ent(ecs, id);
    EntStruct* projectile = get_ent(ecs, proj_id);

    Transform* body = get_comp(ecs, self, Transform);
    Transform* pbody = get_comp(ecs, projectile, Transform);

    if (Vector3Distance(pbody->translation, body->translation) < 2.0f) {
        kill_ent(self);
    }
}

void update_script(Game* game, EcsWorld* ecs, EntStruct* self, EntId id) {
    Script* script = get_comp(ecs, self, Script);

    if (!is_ent_alive(ecs, id)) {
        // Clean up the script
        // free(script->code);
        // script->code = NULL;
        return;
    }

    if (script->which < SCRIPTS_NUM_SCRIPTS && script->which >= 0) {
        // TODO(Dustin):
        // We need to use the pcall version so that we can A. Capture errors B.
        // run larger scripts

        // NOTE(Dustin): janet_call suspends the garbage
        // collector, only use small scripts with this because muh memory

        Janet result;
        janet_dostring(game->env, game->assets->scripts[script->which], "main",
                       &result);

        if (janet_checktype(result, JANET_FUNCTION)) {
            JanetFunction* func = janet_unwrap_function(result);
            JanetFiber* fiber = NULL;
            Janet out;
            JanetSignal sig = janet_pcall(func, 0,
                                          NULL, &out, &fiber);
        }
    }
}

void update_behaviours(Game* game, EcsWorld* ecs, EntId id) {
    EntStruct* self = get_ent(ecs, id);

    if (has_comp(ecs, self, Script)) {
        update_script(game, ecs, self, id);
    }

    if (!has_comp(ecs, self, Actor)) {
        return;
    }

    Actor* actor = get_comp(ecs, self, Actor);

    if (actor->flags & AF_ENEMY) {
        update_enemy(ecs, actor, id);
    }

    switch (actor->type) {
        case ACTOR_END_TARGET: {
            update_end_target(ecs, self, actor);
            break;
        }
        case ACTOR_PINEAPPLE_BOMB: {
            update_pinapple_bomb(ecs, self, actor);
            break;
        }
        default:
            return;
    }
}