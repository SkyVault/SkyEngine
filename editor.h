#ifndef BENIS_EDITOR_H
#define BENIS_EDITOR_H

#if defined _DEBUG

#include "game.h"

typedef struct {
	bool open;
} Ed;

Ed* create_editor();

void update_editor(Ed* editor, Game* game);
void render_editor(Ed* editor, Game* game);
void render_editor_ui(Ed* editor, Game* game);

#endif

#endif//BENIS_EDITOR_H