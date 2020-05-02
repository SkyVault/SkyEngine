#ifndef BENIS_GAME_H
#define BENIS_GAME_H

#include <stdlib.h>

#include "ecs.h"
#include "assets.h"

enum State {
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_QUITTING
};

typedef struct {
    int state;
    Assets* assets;
    Camera* camera;
    EcsWorld* ecs;

    Model skybox;
} Game;

Game* create_game(Assets* assets, Camera* camera, EcsWorld* ent_world);

#endif//BENIS_GAME_H
