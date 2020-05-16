#ifndef BENIS_EDITOR_H
#define BENIS_EDITOR_H

// #if defined _DEBUG

#include "assembler.h"
#include "game.h"
#include "map.h"

enum {
    PLACE_BLOCKS,
    PLACE_ACTORS,
    PLACE_PROPS,
    PLACE_MARKERS,
};

typedef struct Note {
    const char* mesg;
    float time;
    bool active;
} Note;

typedef struct {
    bool open;

    Model models[MESH_NUM_MESHES];

    Note notes[100];
    int num_notes;

    int model;
    int which;
    int y;

    int object_placement_type;

    bool do_export_modal;
    bool do_load_modal;

    int num_maps;
    char** maps;
} Ed;

Ed* create_editor();

void push_message(Ed* self, const char* mesg);

void serialize_map(Ed* editor, Map* map, Game* game, const char* path);
void unserialize_map(Ed* editor, Map* map, Game* game, const char* path);

void update_editor(Ed* editor, Map* map, Game* game);
void render_editor(Ed* editor, Map* map, Game* game);
void render_editor_ui(Ed* editor, Map* map, Game* game);

// #endif

#endif  // BENIS_EDITOR_H