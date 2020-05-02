#include "editor.h"
 
#if defined _DEBUG

#define _px
#define PANEL_RATIO (1.0f/6.0f)
#define MARGIN (8_px)

Ed* create_editor() {
    Ed* editor = malloc(sizeof(Ed));

    if (editor == NULL) {
        printf("Failed to allocate memory for the editor\n");
        exit(EXIT_FAILURE); 
    }



    return editor;
}

void update_editor(Ed* self, Game* game) {
    if (IsKeyPressed(KEY_BACKSLASH))
        self->open = !self->open;

    game->lock_camera = self->open;

    if (!self->open) return;
 
}

void render_editor(Ed* self, Game* game) {
    if (!self->open) return;

}

void render_editor_ui(Ed* self, Game* game) {
    if (!self->open) return;

    DrawRectangle(
        0, 
        0, 
        GetScreenWidth() * PANEL_RATIO, 
        GetScreenHeight(), 
        (Color){0, 0, 0, 100});

}

#endif