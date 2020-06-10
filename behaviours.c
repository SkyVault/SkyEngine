#include "behaviours.h"

Janet move_entity(int32_t argc, Janet* argv) {
    janet_fixarity(argc, 4);

    EntId ent_id = janet_getinteger(argv, 0);
    float delta_x = (float)janet_getnumber(argv, 1);
    float delta_y = (float)janet_getnumber(argv, 2);
    float delta_z = (float)janet_getnumber(argv, 3);

    Game* game = game_get_static_ref();

    EntStruct* self = get_ent(game->ecs, ent_id);
    Transform* transform = get_comp(game->ecs, self, Transform);
    transform->translation.x += delta_x;
    transform->translation.y += delta_y;
    transform->translation.z += delta_z;

    return janet_wrap_nil();
}

Janet get_player_translation(int32_t argc, Janet* argv) {
    janet_fixarity(argc, 0);

    Game* game = game_get_static_ref();

    EntId player = get_first_with(game->ecs, Player);
    Janet* tup = janet_tuple_begin(3);
    tup[0] = tup[1] = tup[2] = janet_wrap_number(0.0f);

    if (is_ent_alive(game->ecs, player)) {
        Vector3 translation =
            get_comp(game->ecs, get_ent(game->ecs, player), Transform)
                ->translation;

        tup[0] = janet_wrap_number(translation.x);
        tup[1] = janet_wrap_number(translation.y);
        tup[2] = janet_wrap_number(translation.z);
    }

    return janet_wrap_tuple(tup);
}

Janet get_translation(int32_t argc, Janet* argv) {
    janet_fixarity(argc, 1);
    EntId id = janet_getinteger(argv, 0);

    Game* game = game_get_static_ref();

    EntStruct* self = get_ent(game->ecs, id);

    Vector3 trans = get_comp(game->ecs, self, Transform)->translation;

    Janet* tup = janet_tuple_begin(3);
    tup[0] = janet_wrap_number(trans.x);
    tup[1] = janet_wrap_number(trans.y);
    tup[2] = janet_wrap_number(trans.z);

    return janet_wrap_tuple(tup);
}

void register_janet_c_functions(Game* game) {
    janet_def(game->env, "move-entity", janet_wrap_cfunction(move_entity),
              NULL);
    janet_def(game->env, "get-player-translation",
              janet_wrap_cfunction(get_player_translation), NULL);
    janet_def(game->env, "get-translation",
              janet_wrap_cfunction(get_translation), NULL);
}

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

        // NOTE(Dustin)[1]: janet_call suspends the garbage
        // collector, only use small scripts with this because muh memory

        // NOTE(Dustin)[2]: We could later use janet_gcroot to keep a
        // JanetFunction from being garbage collected, that way we dont need to
        // re-evaluate the function every single time the entity updates.
        // Also we could stop storing the script as text in the assets.

        Janet* args = janet_tuple_begin(2);
        args[0] = janet_wrap_integer(id);
        args[1] = janet_wrap_number(GetFrameTime());
        janet_tuple_end(args);

        Janet tup = janet_wrap_tuple(args);
        janet_gcroot(tup);

        JanetFiber* fiber = NULL;
        Janet out;
        JanetSignal sig = janet_pcall(game->assets->scripts[script->which], 2,
                                      args, &out, &fiber);

        if (sig == JANET_SIGNAL_ERROR) {
            janet_stacktrace(fiber, out);
        }

        janet_gcunroot(tup);
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