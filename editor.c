#include "editor.h"
 
#if defined _DEBUG

#define PANEL_RATIO (1.0f/6.0f)
#define MARGIN (8)

Ed* create_editor() {
    Ed* editor = malloc(sizeof(Ed));

    if (editor == NULL) {
        printf("Failed to allocate memory for the editor\n");
        exit(EXIT_FAILURE); 
    }

    editor->open = false;
    editor->which = 0;

    return editor;
}

void update_editor(Ed* self, Map* map, Game* game) {
    if (IsKeyPressed(KEY_BACKSLASH))
        self->open = !self->open;

    //game->lock_camera = self->open;

    for (int i = 0; i < MESH_NUM_MESHES; i++) { 
        self->models[i] = LoadModelFromMesh(game->assets->meshes[i]);
    }

    if (!self->open) return;
 
}

void render_editor(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    Vector3 loc = game->camera->target;
    
    Vector3 clamped = (Vector3){
        ((int)loc.x) % CUBE_SIZE,
        ((int)loc.y) % CUBE_SIZE,
        ((int)loc.z) % CUBE_SIZE,
    };

	DrawGrid(1000, CUBE_SIZE);

    DrawCube(loc, 1, 1, 1, RED);

    float occ = (1.0f + cos(GetTime() * 10.0f)) * 0.5f;

    DrawCube(
        (Vector3) { clamped.x * CUBE_SIZE - CUBE_SIZE/2, -1.6, clamped.z * CUBE_SIZE - CUBE_SIZE/2},
        CUBE_SIZE, 0.1f, CUBE_SIZE, (Color) { 
		(int)(occ * 255),
		0,
		(int)(occ * 255),
        100
    });

    DrawLine3D(game->camera->position, loc, RED);

    printf("%f %f %f\n", loc.x, loc.y, loc.z);
}

void render_editor_ui(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    DrawRectangle(
        0, 
        0, 
        GetScreenWidth() * PANEL_RATIO, 
        GetScreenHeight(), 
        (Color){0, 0, 0, 100});

    DrawRectangleLinesEx(
        (Rectangle) {
			0,
            0,
            GetScreenWidth()* PANEL_RATIO,
            GetScreenHeight(),
		},
        2,
        (Color){0, 0, 0, 100}); 
}

#endif
