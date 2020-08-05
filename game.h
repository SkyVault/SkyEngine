#ifndef BENIS_GAME_H
#define BENIS_GAME_H

#include <janet.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_types.h"
#include "scene.h"
#include "assets.h"
#include "ecs.h"
#include "gameworld_types.h"
#include "rlights.h"
#include "particles.h"
#include "main_menu.h"
#include "game_scene.h"

Game* create_game(JanetTable* env);

void update_game(Game *game);
void render_game(Game *game);

void push_scene(Game *self, Scene scene);
void pop_scene(Game *self);
void goto_scene(Game *self, Scene scene);

void goto_scene_using_id(Game *self);

Game* game_get_static_ref();

#endif  // BENIS_GAME_H
