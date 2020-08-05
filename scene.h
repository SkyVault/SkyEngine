#ifndef VAULT_SCENE_H
#define VAULT_SCENE_H

enum SceneState { SCENE_STATE_LOAD, SCENE_STATE_UPDATE, SCENE_STATE_RENDER, SCENE_STATE_DESTROY };

typedef void (*GameSceneFn)(void* data, int state);

typedef struct {
    void* data;
    GameSceneFn func;
} Scene;

#endif//VAULT_SCENE_H
