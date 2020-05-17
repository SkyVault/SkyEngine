#include "editor.h"

// #if defined _DEBUG

#define PANEL_RATIO (1.0f / 6.0f)
#define MARGIN (8)
#define NOTE_HEIGHT (75)

Ed* create_editor() {
    Ed* editor = malloc(sizeof(Ed));

    if (editor == NULL) {
        printf("Failed to allocate memory for the editor\n");
        exit(EXIT_FAILURE);
    }

    editor->open = false;
    editor->num_notes = 0;
    editor->which = 0;
    editor->model = 0;
    editor->y = 0;
    editor->light_panel_y = GetScreenHeight();
    editor->object_placement_type = PLACE_BLOCKS;
    editor->do_export_modal = false;
    editor->do_load_modal = false;
    editor->maps = GetDirectoryFiles("resources/maps/", &editor->num_maps);

    return editor;
}

void push_message(Ed* self, const char* mesg) {
    self->notes[self->num_notes++] = (Note){
        .mesg = mesg,
        .time = 4.0f,
        .active = true,
    };
}

void do_mouse_picking(Ed* self, Map* map, Game* game) {}

void update_editor(Ed* self, Map* map, Game* game) {
    game->noclip = self->open;
    if (IsKeyPressed(KEY_BACKSLASH)) self->open = !self->open;

    if (IsKeyPressed(KEY_E)) {
        self->do_export_modal = true;
    }

    if (!self->open) return;

    for (int i = self->num_notes - 1; i >= 0; i--) {
        Note* note = &self->notes[i];
        if (note->time <= 0) note->active = false;
        if (!note->active) continue;
        note->time -= GetFrameTime();
    }

    for (int i = 0; i < MESH_NUM_MESHES; i++) {
        self->models[i] = LoadModelFromMesh(game->assets->meshes[i]);
    }

    EntId player_id = get_first_with(game->ecs, Player);
    if (player_id >= 0) {
        EntStruct* player = get_ent(game->ecs, player_id);

        if (IsKeyPressed(KEY_SPACE)) {
            self->y++;
            get_comp(game->ecs, player, Transform)->translation.y =
                self->y * (float)CUBE_SIZE;
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

    int ms = map->num_models;

    if (self->object_placement_type == PLACE_PROPS) {
        ms = sizeof(prop_types) / sizeof(prop_types[0]);
    }

    if (self->model >= ms) {
        self->model = 0;
    }

    if (self->model < 0) {
        self->model = ms - 1;
    }
}

void render_editor(Ed* self, Map* map, Game* game) {
    if (!self->open) return;

    Vector3 loc = game->camera->target;

    static float dist = 1.0f;
    dist += GetMouseWheelMove() * 0.05f;

    // loc.x *= dist;
    // loc.y *= dist;
    // loc.z *= dist;

    const int cs = CUBE_SIZE;

    Vector3 clamped = (Vector3){((int)(loc.x / cs) * cs) + cs / 2, cs * self->y,
                                ((int)(loc.z / cs) * cs) + cs / 2};

    float occ = (1.0f + cosf((float)GetTime() * 10.0f)) * 0.5f;

    DrawCylinder((Vector3){map->player_x, -3.0f, map->player_z}, 1.0f, 1.0f,
                 4.0f, 20, (Color){0, 100, 255, 100});

    if (self->object_placement_type == PLACE_BLOCKS) {
        DrawModel(
            map->models[self->model], clamped, CUBE_SIZE,
            (Color){(unsigned char)(occ * 255), (unsigned char)(occ * 255),
                    (unsigned char)(occ * 255), 100});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->lock_camera) {
            // Try to intersect

            int index =
                (int)(clamped.x / cs) + (int)(clamped.z / cs) * map->width;

            if (index < MAX_MAP_WIDTH * MAX_MAP_HEIGHT && index >= 0) {
                map->walls[self->y][index].active =
                    !map->walls[self->y][index].active;

                if (!map->walls[self->y][index].active)
                    map->walls[self->y][index].model = 0;
                else
                    map->walls[self->y][index].model = self->model;
            }
        }
    } else if (self->object_placement_type == PLACE_ACTORS) {
        Texture2D tex = game->assets->textures[TEX_GIRL_1 + self->model];
        DrawBillboard(*game->camera, tex, loc, CUBE_SIZE, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->lock_camera) {
            assemble(ACTOR_GIRL_1 + self->model, game, loc.x, loc.y, loc.z, 0,
                     0);
            map->spawns[map->num_spawns++] = (ActorSpawn){
                .type = ACTOR_GIRL_1 + self->model,
                .position = (Vector3){loc.x, loc.y, loc.z},
            };
        }
    } else if (self->object_placement_type == PLACE_PROPS) {
        Texture2D tex = game->assets->textures[TEX_PROPS];

        Prop prop = prop_types[self->model];
        prop.position = loc;

        DrawBillboardRec(*game->camera, tex, prop.region, loc, prop.scale,
                         WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            printf("added prop\n");
            map->props[map->num_props++] = prop;
        }
    } else if (self->object_placement_type == PLACE_MARKERS) {
        DrawCylinder((Vector3){loc.x, -3.0f, loc.z}, 1.0f, 1.0f, 4.0f, 20,
                     (Color){0, 100, 255, 100});

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            map->player_x = loc.x;
            map->player_z = loc.z;
        }
    }
}

#define MIN3(a, b, c) \
    ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char* s1, char* s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int* column = talloc(sizeof(unsigned int) * s1len + 1);
    for (y = 1; y <= s1len; y++) column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1,
                             lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return (column[s1len]);
}

static char load_buff[1024] = {'\0'};

int sort_levenshtein(const void* a, const void* b) {
    char* aa = (char*)a;
    char* bb = (char*)b;
    int l1 = levenshtein(load_buff, aa);
    int l2 = levenshtein(load_buff, bb);
    if (l1 > l2) return 1;
    if (l1 < l2) return -1;
    if (l1 == l2) return 0;
    return 0;
}

void render_editor_ui(Ed* self, Map* map, Game* game) {
    Shader* shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

    int id = 200;

    if (!self->open) {
        if (DoBtn(id++, GetScreenWidth() / 2 - 50.0f, 0, 100, 25, "Editor")) {
            push_message(self, "Editor mode!");
            self->open = true;
        }
        return;
    }

    if (DoBtn(id++, GetScreenWidth() / 2 - 50.0f, 0, 100, 25, "Play")) {
        self->open = false;
    }

    // Notifications
    int last = self->object_placement_type;
    self->object_placement_type =
        DoToggleGroupV(id++, "BLOCKS|ACTORS|BILLBOARDS|MARKERS|", 0,
                       GetScreenHeight() - (self->placement_toggle_height + 50),
                       &self->placement_toggle_height);

    // Draws a label for the type of marker
    // TODO(Dustin): Add new markers for exit and other stuff
    if (self->object_placement_type == PLACE_MARKERS) {
        DoCenterXLabel(id++, (float)GetScreenWidth(),
                       (float)GetScreenHeight() / 2 + 50.0f, 30,
                       "Player start");
    }

    // Map loading and unloading
    if (DoBtn(id++, 100, GetScreenHeight() - 50.0f, 100, 50, "Export")) {
        self->do_export_modal = true;
    }

    if (DoBtn(id++, 0, GetScreenHeight() - 50.0f, 100, 50, "Load")) {
        self->do_load_modal = true;
    }

    if (self->do_load_modal) {
        DoModal();
        Unlock();

        // TODO(Dustin): Get this to work
        // qsort(self->maps, self->num_maps, sizeof(char*), sort_levenshtein);

        if (DoTextInput(id++, load_buff, 1024, GetScreenWidth() / 2 - 150.f,
                        GetScreenHeight() / 2 - 25.f, 300, 50)) {
            tstr path = talloc(512);
            sprintf(path, "resources/maps/%s.janet", load_buff);

            if (FileExists(path) == false) {
                push_message(self,
                             FormatText("File [%s] does not exist", path));
            } else {
                game->map = load_map_from_script(path, game);
            }

            self->do_load_modal = false;
        }

        for (int i = 2; i < self->num_maps; i++) {
            if (DoBtn(id++, GetScreenWidth() / 2 - 150.0f,
                      GetScreenHeight() / 2 + 20 + 30 * (i - 1.0f), 300, 30,
                      self->maps[i])) {
                const char* path =
                    TextFormat("resources/maps/%s", self->maps[i]);
                if (FileExists(path)) {
                    game->map = load_map_from_script(path, game);
                    self->do_load_modal = false;
                }
            }
        }

        if (DoBtn(id++, GetScreenWidth() / 2 + 200.0f, GetScreenHeight() / 2.0f,
                  30, 30, "X")) {
            self->do_load_modal = false;
        }

        Lock();
    } else if (self->do_export_modal) {
        DoModal();
        Unlock();

        DoCenterXLabel(id++, (float)GetScreenWidth(),
                       (float)GetScreenHeight() / 2 - 100, 30, "Map Name");

        static char buffer[100] = {'\0'};
        if (DoTextInput(id++, buffer, 100, GetScreenWidth() / 2 - 150.0f,
                        GetScreenHeight() / 2 - 25.0f, 300, 50)) {
            tstr path = talloc(512);
            sprintf(path, "resources/maps/%s.janet", buffer);
            serialize_map(self, map, game, path);
            push_message(self, FormatText("Exported [%s]", path));
            self->do_export_modal = false;
        }

        if (DoBtn(id++, GetScreenWidth() / 2.f + 200, GetScreenHeight() / 2.0f,
                  30, 30, "X")) {
            self->do_export_modal = false;
        }

        Lock();
    } else {
        Unlock();
    }

    if (last != self->object_placement_type) {
        self->model = 0;
    }

    // Lights
    DoPanel(id++, GetScreenWidth() - 400.0f, self->light_panel_y + 30, 400,
            GetScreenHeight());

    const float panel_w = 400;
    const float lx = GetScreenWidth() - panel_w;

    float cursor_y = self->light_panel_y;
    if (DoBtn(id++, lx, cursor_y, 200, 30,
              TextFormat("%s Lights", (self->do_lights_panel ? "+" : "-")))) {
        self->do_lights_panel = !self->do_lights_panel;
    }

    if (!self->do_lights_panel) {
        self->light_panel_y =
            lerp(GetFrameTime() * 10.0f, self->light_panel_y, 30);
        // Do lights panel

        cursor_y += 30;

        if (DoBtn(id++, lx + 2, cursor_y + 2, 30, 30, "+")) {
            Light* light = &map->lights[map->num_lights++];
            light->enabled = true;
            light->color = WHITE;
            UpdateLightValues(game->assets->shaders[SHADER_PHONG_LIGHTING],
                              *light);
        }

        cursor_y += 32 + 2;

        for (int i = 0; i < map->num_lights; i++) {
            Light light = map->lights[i];

            if (DoCollapsingHeader(id++, TextFormat("%s[%d]", "Light", i),
                                   lx + 10, cursor_y, panel_w - 20, 40)) {
                cursor_y += 40 + 2;

                DoLabel(id++, "Disabled", lx + 30, cursor_y, 100, 30, 30);
                map->lights[i].enabled =
                    !DoCheckBox(id++, lx + 30 + 100, cursor_y, 30, 30);
                cursor_y += 32;

                DoLabel(id++, "X", lx + 30, cursor_y, 40, 30, 30);
                float x =
                    DoSlider(id++, lx + 30 + 42, cursor_y, 200, 30, 0, 100);
                cursor_y += 32;

                DoLabel(id++, "Y", lx + 30, cursor_y, 40, 30, 30);
                float y =
                    DoSlider(id++, lx + 30 + 42, cursor_y, 200, 30, 0, 100);
                cursor_y += 32;

                DoLabel(id++, "Z", lx + 30, cursor_y, 40, 30, 30);
                float z =
                    DoSlider(id++, lx + 30 + 42, cursor_y, 200, 30, 0, 100);

                cursor_y += 32;

                DoLabel(id++, "Color", lx + 30, cursor_y, 100, 30, 30);
                DrawRectangle(lx + 30 + 100, cursor_y, 30, 30,
                              LightColors[map->light_color[i]]);

                if (DoClickRegion(id++, lx + 30 + 100, cursor_y, 30, 30,
                                  light.color)) {
                    map->light_color[i]++;
                    map->light_color[i] %=
                        (sizeof(LightColors) / sizeof(LightColors[0]));
                    map->lights[i].color = LightColors[map->light_color[i]];
                }

                cursor_y += 32;

                // map->lights[i].position = (Vector3){x, y, z};
                UpdateLightValues(*shader, map->lights[i]);

            } else {
                cursor_y += 40 + 2;
            }
        }

    } else {
        self->light_panel_y = lerp(GetFrameTime() * 10.0f, self->light_panel_y,
                                   GetScreenHeight() - 30);
    }

    id += 1000;

    int y = 0;
    for (int i = self->num_notes - 1; i >= 0; i--) {
        Note note = self->notes[i];

        if (!note.active) continue;

        Rectangle r = {(float)GetScreenWidth() - 200.0f,
                       (float)GetScreenHeight() - (NOTE_HEIGHT * (y + 1.0f)),
                       200, NOTE_HEIGHT};
        DoFrame(id++, r.x - 4.0f, r.y - 4.0f - ((y + 1.0f) * 4.0f),
                r.width + 8.0f, r.height + 8.0f);
        DoLabel(id - 1, note.mesg, r.x, r.y - ((y + 1.0f) * 4.0f), r.width,
                r.height, 20);
        y++;
    }
}

void serialize_map(Ed* editor, Map* map, Game* game, const char* path) {
    const int memsize = (1024 * 1000);  // Allocate 1m for the output buffer
    char* builder = malloc(sizeof(char) * memsize);
    char* it = builder;
    char* start = it;
    memset(builder, '\0', sizeof(char) * memsize);

    it += sprintf(it, "@{ :size @[%d %d]\n   :start @[%f %f]\n   :layers @[",
                  map->width, map->height, map->player_x, map->player_z);

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

    it += sprintf(it, "]\n   :props @[");

    for (int prop = 0; prop < map->num_props; prop++) {
        Prop p = map->props[prop];
        it += sprintf(it, "@[%f %f %f %f %f %f %f %f]\n", p.region.x,
                      p.region.y, p.region.width, p.region.height, p.position.x,
                      p.position.y, p.position.z, p.scale);
    }

    it += sprintf(it, "]\n   :lights @[");

    for (int i = 0; i < MAX_LIGHTS; i++) {
        Light light = map->lights[i];

        it += sprintf(it, "\n      @[%s  %f %f %f  %d]",
                      (light.enabled ? "true" : "false"), light.position.x,
                      light.position.y, light.position.z, map->light_color[i]);
    }

    it += sprintf(it, "]\n   :actors @[");

    for (int i = 0; i < map->num_spawns; i++) {
        ActorSpawn s = map->spawns[i];
        it += sprintf(it, "\n      @[%d  %f %f %f]", s.type - ACTOR_GIRL_1,
                      s.position.x, s.position.y, s.position.z);
    }

    it += sprintf(it, "]}");

    const size_t bytes = strlen(it);
    printf("Written b: [%zu] kb: [%zu]\n", bytes, bytes / 2);

    FILE* f = fopen(path, "w");
    fwrite(builder, sizeof(char), strlen(builder), f);
    fclose(f);

    return;
}

void unserialize_map(Ed* editor, Map* map, Game* game, const char* path) {}

// #endif
