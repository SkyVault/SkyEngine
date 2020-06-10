#include "game.h"

static Game* static_ref = NULL;

Game* create_game(Assets* assets, Camera* camera, EcsWorld* ecs,
                  JanetTable* env) {
    Game* game = malloc(sizeof(Game));
    game->state = STATE_RUNNING;
    game->scene = SCENE_MAIN_MENU;
    game->assets = assets;
    game->camera = camera;
    game->ecs = ecs;
    game->lock_camera = false;
    game->editor_open = false;
    game->env = env;
    game->noclip = false;
    static_ref = game;
    return game;
}

Game* game_get_static_ref() { return static_ref; }

void update_game(Game* game) {
    static_ref = game;
    if (game->lock_camera) {
        EnableCursor();
    } else {
        DisableCursor();
    }
}
