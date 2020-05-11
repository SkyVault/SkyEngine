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
    editor->model = 0;
    editor->y = 0;

    return editor;
}

void do_mouse_picking(Ed* self, Map* map, Game* game) {}

void update_editor(Ed* self, Map* map, Game* game) {
    if (IsKeyPressed(KEY_BACKSLASH)) self->open = !self->open;

    if (IsKeyPressed(KEY_E)) {
        serialize_map(self, map, game, "resources/maps/edit.janet");
    }

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
                self->y * (float)CUBE_SIZE;
        }
    }

    int sc = GetMouseWheelMove();

    if (sc != 0) {
        if (sc > 0) self->model++;
        if (sc < 0) self->model--;
    }

    if (self->model >= map->num_models) {
        self->model = 0;
    }

    if (self->model < 0) {
        self->model = map->num_models - 1;
    }
}

void render_editor(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    Vector3 loc = game->camera->target;
    Vector3 norm = Vector3Normalize(loc);

    static float dist = 1.0f;
    dist += GetMouseWheelMove() * 0.05f;

    // loc.x *= dist;
    // loc.y *= dist;
    // loc.z *= dist;

    const int cs = CUBE_SIZE;

    Vector3 clamped = (Vector3){((int)(loc.x / cs) * cs) + cs / 2, cs * self->y,
                                ((int)(loc.z / cs) * cs) + cs / 2};

    float occ = (1.0f + cosf(GetTime() * 10.0f)) * 0.5f;

    DrawModel(map->models[self->model], clamped, CUBE_SIZE,
              (Color){occ * 255, occ * 255, occ * 255, 100});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->lock_camera) {
        // Try to intersect

        int index = (int)(clamped.x / cs) + (int)(clamped.z / cs) * map->width;

        if (index < MAX_MAP_WIDTH * MAX_MAP_HEIGHT && index >= 0) {
            map->walls[self->y][index].active =
                !map->walls[self->y][index].active;

            if (!map->walls[self->y][index].active)
                map->walls[self->y][index].model = 0;
            else
                map->walls[self->y][index].model = self->model;
        }
    }
}

void render_editor_ui(Ed* self, Map* map, Game* game) {
    if (!self->open) return;
    static char buffer[100] = {'\0'};
    DoTextInput(100, buffer, 100 * sizeof(char), 0, GetScreenHeight() - 50, 300,
                50);
}

void serialize_map(Ed* editor, Map* map, Game* game, const char* path) {
    const int memsize = 2048 * 8;
    char* builder = malloc(sizeof(char) * memsize);
    char* it = builder;
    memset(builder, '\0', sizeof(char) * memsize);

    it += sprintf(it, "@{ :size @[%d %d]\n   :layers @[", map->width,
                  map->height);

    for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
        it += sprintf(it, "\n      @{ :data ``");

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                Wall wall = map->walls[layer][x + y * map->width];
                if (wall.active) {
                    it += sprintf(it, "%d", wall.model + 1);
                } else {
                    it += sprintf(it, ".");
                }
            }
        }
        it += sprintf(it, "`` }");
    }

    it += sprintf(it, "]}");

    FILE* f = fopen(path, "w");
    fwrite(builder, sizeof(char), strlen(builder), f);
    fclose(f);

    return;
}

void unserialize_map(Ed* editor, Map* map, Game* game, const char* path) {}

#endif
