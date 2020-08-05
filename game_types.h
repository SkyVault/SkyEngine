#ifndef VAULT_GAME_H
#define VAULT_GAME_H

#include <janet.h>
#include <stdio.h>
#include <stdlib.h>

#include "renderer.h"
#include "scene.h"
#include "assets.h"
#include "ecs.h"
#include "gameworld_types.h"
#include "rlights.h"
#include "particles.h"

enum State { STATE_RUNNING, STATE_PAUSED, STATE_QUITTING };
enum Scenes { SCENE_MAIN_MENU, SCENE_GAME, SCENE_EDIT, SCENE_SETTINGS };

typedef struct {
    Assets* assets;
    Camera* camera;
    EcsWorld* ecs;
    Region* map;
    ParticleSystem *particle_sys;
    GfxState *gfx;

    Scene scenes[16]; // Allows for a stack of 16 scenes, maybe need to grow this but I highly doubt it
    int num_scenes;

    JanetTable* env;

    Model skybox;

    bool lock_camera;
    bool editor_open;

    int state;
    int scene;

    bool noclip;
} Game;

#endif//VAULT_GAME_H
