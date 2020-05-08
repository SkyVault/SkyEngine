#include "game.h"

Game* create_game(Assets* assets, Camera* camera, EcsWorld* ecs) {
    Game* game = malloc(sizeof(Game));
    game->state = STATE_RUNNING;
    game->scene = SCENE_GAME;
    game->assets = assets;
    game->camera = camera;
    game->ecs = ecs;
    game->lock_camera = false;
    return game;
}

void update_game(Game* game) {
    if (game->lock_camera) {
        EnableCursor();
    } else {
        DisableCursor();
    }
}
