#ifndef BENIS_EDITOR_H
#define BENIS_EDITOR_H

// #if defined _DEBUG

#include "assembler.h"
#include "game.h"
#include "gameworld.h"

#ifdef _WIN32
// #include <Windows.h>
#endif

enum {
    PLACE_NONE,
    PLACE_BLOCKS,
    PLACE_ACTORS,
    PLACE_PROPS,
    PLACE_MARKERS,
};

enum {
    MARKER_PLAYER_START,
    MARKER_EXIT,
    MARKER_NUM_MARKERS,
};

// use for tabs next to the lighting panel
enum {
    SHOWING_LIGHT_PANEL,
    SHOWING_EXIT_PANEL,
    SHOWING_NOTHING,
};

typedef struct Note {
    const char* mesg;
    float time;
    bool active;
} Note;

enum { MAX_HISTORY = 256 };

enum {
    EDITOR_STATE_NONE,
    EDITOR_STATE_EXPORT_MODAL,
    EDITOR_STATE_EXIT_PLACEMENT_MODAL,
    EDITOR_STATE_LOAD_MODAL,

    EDITOR_STATE_LIGHTS_MODAL,
    EDITOR_STATE_MODEL_SELECTOR_MODAL,

    EDITOR_STATE_CONSOLE,
};
typedef struct {
    bool open;

    Model models[MESH_NUM_MESHES];

    Note notes[100];
    int num_notes;

    int editing_exit;

    int model;
    int which;
    int y;
    int light_grabbed;

    int which_marker;

    float lights_panel_y;
    float models_panel_y;

    float console_y;
    float placement_toggle_height;

    int object_placement_type;

    int state;

    bool do_console;

    int num_maps;
    char** maps;

    int history_size;
    char* history[MAX_HISTORY];
} Ed;

Ed* create_editor();

void push_message(Ed* self, const char* mesg);

void serialize_map(Ed* editor, Region* map, Game* game, const char* path);
void unserialize_map(Ed* editor, Region* map, Game* game, const char* path);

void update_editor(Ed* editor, Region* map, Game* game);
void render_editor(Ed* editor, GfxState* gfx, Region* map, Game* game);
void render_editor_ui(Ed* editor, GfxState* gfx, Region* map, Game* game);

// #endif

#endif  // BENIS_EDITOR_H