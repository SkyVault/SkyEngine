#ifndef VAULT_GAME_SCENE_H
#define VAULT_GAME_SCENE_H

#include <GL/glew.h>

#include "game_types.h"
#include "assembler.h"
#include "editor.h"

struct Ed;
typedef struct {
    Game *game;
    struct Ed *editor;
} GameSceneState;

void game_scene_function(void *data, int state);

#endif//VAULT_GAME_SCENE_H
