#ifndef BENIS_EDITOR_H
#define BENIS_EDITOR_H

#if defined _DEBUG

#include "assembler.h"
#include "game.h"
#include "map.h"

typedef struct {
    bool open;

    Model models[MESH_NUM_MESHES];
    int model;

    int which;
    int y;
} Ed;

Ed* create_editor();

void serialize_map(Ed* editor, Map* map, Game* game, const char* path);
void unserialize_map(Ed* editor, Map* map, Game* game, const char* path);

void update_editor(Ed* editor, Map* map, Game* game);
void render_editor(Ed* editor, Map* map, Game* game);
void render_editor_ui(Ed* editor, Map* map, Game* game);

#endif

#endif  // BENIS_EDITOR_H