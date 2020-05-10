#include "editor.h"

#if defined _DEBUG

#define PANEL_RATIO (1.0f / 6.0f)
#define MARGIN (8)

Ed* create_editor() {
    Ed* editor = malloc(sizeof(Ed));

    if (editor == NULL) {
        printf("Failed to allocate memory for the editor\n");
        exit(EXIT_FAILURE);
    }

    editor->open = false;
    editor->which = 0;
    editor->y = 0;

    return editor;
}

void do_mouse_picking(Ed* self, Map* map, Game* game) {}

void update_editor(Ed* self, Map* map, Game* game) {
    if (IsKeyPressed(KEY_BACKSLASH)) self->open = !self->open;

    if (!self->open) return;

    // game->lock_camera = self->open;

    for (int i = 0; i < MESH_NUM_MESHES; i++) {
        self->models[i] = LoadModelFromMesh(game->assets->meshes[i]);
    }

    EntId player_id = get_first_with(game->ecs, Player);
    if (player_id >= 0) {
        EntStruct* player = get_ent(game->ecs, player_id);

        if (IsKeyPressed(KEY_SPACE)) {
            self->y++;
            get_comp(game->ecs, player, Transform)->translation.y =
                self->y * CUBE_SIZE;
        }

        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            self->y--;
            if (self->y < 0) self->y = 0;
            get_comp(game->ecs, player, Transform)->translation.y =
                self->y * CUBE_SIZE;
        }
    }
}

void render_editor(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    Vector3 loc = game->camera->target;
    Vector3 norm = Vector3Normalize(loc);

    static float dist = 1.0f;
    dist += GetMouseWheelMove() * 0.05f;

    loc.x *= dist;
    loc.y *= dist;
    loc.z *= dist;

    const int cs = CUBE_SIZE;

    Vector3 clamped = (Vector3){((int)(loc.x / cs) * cs) + cs / 2, 0.0f,
                                ((int)(loc.z / cs) * cs) + cs / 2};

    DrawGrid(1000, CUBE_SIZE);

    float occ = (1.0f + cosf(GetTime() * 10.0f)) * 0.5f;
    DrawCube((Vector3){clamped.x, -cs / 2 + cs * self->y, clamped.z},
             CUBE_SIZE + 0.01f, CUBE_SIZE / 2, CUBE_SIZE + 0.01f,
             (Color){occ * 255, 0, occ * 255, 50});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Try to intersect

        int index = (int)(clamped.x / cs) + (int)(clamped.z / cs) * map->width;

        if (index < MAX_MAP_WIDTH * MAX_MAP_HEIGHT && index >= 0) {
            map->walls[self->y][index].active =
                !map->walls[self->y][index].active;
            map->walls[self->y][index].model = 0;
        }
    }
}

void render_editor_ui(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    // DrawRectangle(0, 0, GetScreenWidth() * PANEL_RATIO, GetScreenHeight(),
    //               (Color){0, 0, 0, 100});

    // DrawRectangleLinesEx(
    //     (Rectangle){
    //         0,
    //         0,
    //         GetScreenWidth() * PANEL_RATIO,
    //         GetScreenHeight(),
    //     },
    //     2, (Color){0, 0, 0, 100});
}

#endif
