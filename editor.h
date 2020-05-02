#ifndef BENIS_EDITOR_H
#define BENIS_EDITOR_H

#if defined _DEBUG

#include "game.h"
#include "map.h"
#include "physics.h"

typedef struct {
	bool open;

	Model models[MESH_NUM_MESHES];

	int which;
} Ed;

Ed* create_editor();

void update_editor(Ed* editor, Map* map, Game* game);
void render_editor(Ed* editor, Map* map, Game* game);
void render_editor_ui(Ed* editor, Map* map, Game* game);

#endif

#endif//BENIS_EDITOR_H