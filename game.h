#ifndef BENIS_GAME_H
#define BENIS_GAME_H

#include <janet.h>
#include <stdlib.h>

#include "assets.h"
#include "ecs.h"

enum State { STATE_RUNNING, STATE_PAUSED, STATE_QUITTING };
enum Scenes { SCENE_MAIN_MENU, SCENE_GAME, SCENE_SETTINGS };

typedef struct {
    Assets* assets;
    Camera* camera;
    EcsWorld* ecs;

    JanetTable* env;

    Model skybox;

    bool lock_camera;
    int state;
    int scene;

    bool noclip;
} Game;

Game* create_game(Assets* assets, Camera* camera, EcsWorld* ent_world,
                  JanetTable* env);

void update_game(Game* game);

#endif  // BENIS_GAME_H
