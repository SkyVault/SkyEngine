#include "editor.h"
#include "sb.h"

// #if defined _DEBUG

#define PANEL_RATIO (1.0f / 6.0f)
#define MARGIN (8)
#define NOTE_HEIGHT (75)

bool get_mouse_placement_loc(Game *game, float y, Vector3 *loc);

// TODO(Dustin): Refactor this somehow
static Game *game_s = NULL;
static Ed *editor_s = NULL;
static Region *map_s = NULL;

#define PANEL_WIDTH (400)
#define PANEL_BTN_WIDTH (100)
#define PANEL_BTN_HEIGHT (20)
#define PANEL_X (GetScreenWidth() - PANEL_WIDTH)

// TODO(Dustin):
//  Create a on exit save modal
// That way we can remove the statics above and just have the editor

void on_exit(void) {
  // serialize_map(editor_s, map_s, game_s, "resources/maps/unsaved-backup");
}

Ed *create_editor() {
  Ed *editor = malloc(sizeof(Ed));

  if (editor == NULL) {
    printf("Failed to allocate memory for the editor\n");
    exit(EXIT_FAILURE);
  }

  editor->open = false;
  editor->num_notes = 0;
  editor->which = 0;
  editor->model = 0;
  editor->y = 0;

  editor->lights_panel_y = (float)GetScreenHeight();
  editor->models_panel_y = (float)GetScreenHeight();
  editor->node_tree_panel_y = (float)GetScreenHeight();

  editor->object_placement_type = PLACE_NODES;

  editor->state = EDITOR_STATE_NONE;

  editor->maps = GetDirectoryFiles("resources/maps/", &editor->num_maps);
  editor->which_marker = MARKER_PLAYER_START;
  editor->console_y = 0.0f;
  editor->do_console = false;

  editor->history_size = 0;

  editor->editing_exit = -1;
  editor->light_grabbed = -1;

  editor->selected_node = NULL;

  editor->do_selected_node_panel = false;

  // Exit handler, portable?
  atexit(on_exit);

  return editor;
}

void push_message(Ed *self, const char *mesg) {
  self->notes[self->num_notes++] = (Note){
      .mesg = mesg,
      .time = 4.0f,
      .active = true,
  };
}

Node *check_if_clicked(Ray ray, Node *node) {
  if (node == NULL)
    return node;

  if (node->type == NODE_TYPE_MODEL) {
    BoundingBox box = MeshBoundingBox(node->model.meshes[0]);

    Vector3 pos = get_transform_from_node(node).translation;
    box.min = Vector3Add(box.min, pos);
    box.max = Vector3Add(box.max, pos);

    if (CheckCollisionRayBox(ray, box)) {
      if (GetCollisionRayModel(ray, node->model).hit) {
        return node;
      }
    }
  }

  if (node->child) {
    Node *child = check_if_clicked(ray, node->child);
    if (child != NULL)
      return child;
  }

  if (node->next) {
    Node *next = check_if_clicked(ray, node->next);
    if (next != NULL)
      return next;
  }

  return NULL;
}

void do_mouse_picking(Ed *self, Region *map, Game *game) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && map->scene_root != NULL) {
    Ray ray = GetMouseRay(
        (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2}, *game->camera);
    self->selected_node = check_if_clicked(ray, map->scene_root);
  }
}

void push_command_output(Ed *self, const char *str) {
  char *copy = malloc(strlen(str));
  sprintf(copy, "%s", str);

  self->history[self->history_size++] = copy;
}

void render_console(Ed *self, Region *map, Game *game, int id) {
  const float speed = 10.0f;
  if (self->do_console) {
    self->console_y =
        lerp_t(self->console_y, GetScreenHeight() / 4, speed * GetFrameTime());
  } else {
    self->console_y = lerp_t(self->console_y, 0, speed * GetFrameTime());
  }

  float cursor_y = -GetScreenHeight() / 4 + self->console_y;
  float height = GetScreenHeight() / 4;

  do_frame(0, cursor_y, GetScreenWidth(), GetScreenHeight() / 4, 0.8f);

  static char input[512] = {'\0'};
  static int input_cursor = 0;
  do_text_input(input, &input_cursor, 512, 0, cursor_y + height - 32,
                GetScreenWidth(), 32);

  if (IsKeyPressed(KEY_ENTER)) {
    push_command_output(self, input);
  }

  const float font_size = 30;

  float y = 0;
  for (int i = self->history_size - 1; i >= 0; i--) {
    const float yy = self->console_y - (32 + font_size + y);
    DrawRectangle(0, yy, GetScreenWidth(), font_size, (Color){0, 0, 0, 100});
    DrawTextEx(GetFont(), FormatText("[%d]", i), (Vector2){2, yy}, font_size, 1,
               WHITE);
    do_label(self->history[i], 75, yy, GetScreenWidth() - 2, font_size,
             font_size);
    y += 34;
  }
}

void toggle_model_selector_modal(Ed *self) {
  if (self->state != EDITOR_STATE_MODEL_SELECTOR_MODAL)
    self->state = EDITOR_STATE_MODEL_SELECTOR_MODAL;
  else
    self->state = EDITOR_STATE_NONE;
}

void toggle_model_node_tree_modal(Ed *self) {
  if (self->state != EDITOR_STATE_NODE_TREE_MODAL)
    self->state = EDITOR_STATE_NODE_TREE_MODAL;
  else
    self->state = EDITOR_STATE_NONE;
}

void update_editor(Ed *self, Region *map, Game *game) {
  // Refactor
  editor_s = self;
  map_s = map;
  game_s = game;

  game->noclip = self->open;
  if ((IsKeyPressed(KEY_TAB) && IsKeyDown(KEY_LEFT_SHIFT)))
    self->open = !self->open;

  game->editor_open = self->open;

  // if (IsKeyPressed(KEY_E)) {
  //     self->state = EDITOR_STATE_EXPORT_MODAL;
  // }

  do_mouse_picking(self, map, game);

  if (IsKeyPressed(KEY_E)) {
    toggle_model_selector_modal(self);
    self->object_placement_type =
        (self->state == EDITOR_STATE_MODEL_SELECTOR_MODAL ? PLACE_NODES
                                                          : PLACE_NONE);
    game->lock_camera = true;

    if (self->state != EDITOR_STATE_MODEL_SELECTOR_MODAL)
      game->lock_camera = false;
  }

  if (IsKeyPressed(KEY_T) && !game->lock_camera) {
    self->do_console = !self->do_console;
  }

  if (!self->open)
    return;

  for (int i = self->num_notes - 1; i >= 0; i--) {
    Note *note = &self->notes[i];
    if (note->time <= 0)
      note->active = false;
    if (!note->active)
      continue;
    note->time -= GetFrameTime();
  }

  for (int i = 0; i < MESH_NUM_MESHES; i++) {
    self->models[i] = LoadModelFromMesh(game->assets->meshes[i]);
  }

  EntId player_id = get_first_with(game->ecs, Player);
  if (player_id >= 0) {
    EntStruct *player = get_ent(game->ecs, player_id);

    if (IsKeyPressed(KEY_SPACE)) {
      self->y++;
      get_comp(game->ecs, player, Transform)->translation.y =
          self->y * (float)CUBE_SIZE + (GLOBAL_SCALE - ACTOR_HEIGHT);
    }

    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
      self->y--;
      if (self->y < 0)
        self->y = 0;
      get_comp(game->ecs, player, Transform)->translation.y =
          self->y * (float)CUBE_SIZE + (ACTOR_HEIGHT - GLOBAL_SCALE);
    }
  }

  int sc = GetMouseWheelMove();

  if (sc != 0 && !IsMouseOnUiElement()) {
    if (self->object_placement_type == PLACE_MARKERS) {
      if (sc > 0)
        self->which_marker++;
      if (sc < 0)
        self->which_marker--;
      self->which_marker %= (MARKER_NUM_MARKERS);
    } else {
      if (sc > 0)
        self->model++;
      if (sc < 0)
        self->model--;
    }
  }

  int ms = map->num_models;

  if (self->object_placement_type == PLACE_PROPS) {
    ms = sizeof(prop_types) / sizeof(prop_types[0]);
  }

  // ROTATION WITH MIDDLE MOUSE
  if (self->selected_node != NULL) {
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
      self->rot_mouse_start = GetMousePosition();
    } else if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
      Vector2 delta =
          Vector2Subtract(self->rot_mouse_start, GetMousePosition());

      self->selected_node->transform.rotation.y += delta.x;
    }
  }

  {
    // Deleting nodes
    if (IsKeyPressed(KEY_DELETE)) {
      if (self->selected_node != NULL) {
      }
    }
  }

  if (self->selected_node != NULL) {
    self->do_selected_node_panel = true;
  } else {
    self->do_selected_node_panel = false;
  }

  if (self->model >= ms) {
    self->model = 0;
  }

  if (self->model < 0) {
    self->model = ms - 1;
  }

  if (self->light_grabbed >= 0) {
    // TODO(Dustin): move this to a seperate function
    Vector3 loc = Vector3Zero();
    bool hit =
        get_mouse_placement_loc(game, self->y * (float)(GLOBAL_SCALE), &loc);

    game->assets->lights[self->light_grabbed].position = loc;
    UpdateLightValues(game->assets->shaders[SHADER_PHONG_LIGHTING],
                      game->assets->lights[self->light_grabbed]);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      self->light_grabbed = -1;
    }
  }
}

bool get_mouse_placement_loc(Game *game, float y, Vector3 *loc) {
  Ray ray =
      GetMouseRay((Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2 - 50},
                  *game->camera);

  RayHitInfo nearestHit = {0};
  nearestHit.distance = FLOAT_MAX;
  nearestHit.hit = false;

  // Check ray collision aginst ground plane
  RayHitInfo groundHitInfo = GetCollisionRayGround(ray, 0.0f);

  if ((groundHitInfo.hit) && (groundHitInfo.distance < nearestHit.distance)) {
    nearestHit = groundHitInfo;
  }

  if (nearestHit.hit) {
    *loc = nearestHit.position;
    return true;
  }

  return false;
}

void draw_node_tree_bounding_boxes(Node *node) {
  if (node == NULL)
    return;

  if (node->type == NODE_TYPE_MODEL) {
    Transform trans = get_transform_from_node(node);
    Vector3 scale = trans.scale;
    Quaternion rot = trans.rotation;
    Vector3 pos = trans.translation;

    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixScale(scale.x, scale.y, scale.z));
    m = MatrixMultiply(m, QuaternionToMatrix(rot));
    m = MatrixMultiply(m, MatrixTranslate(pos.x, pos.y, pos.z));
    node->model.transform = m;

    BoundingBox box = MeshBoundingBox(node->model.meshes[0]);
    box.min = Vector3Add(box.min, pos);
    box.max = Vector3Add(box.max, pos);

    DrawBoundingBox(box, (Color){255, 0, 255, 200});
  }

  if (node->child)
    draw_node_tree_bounding_boxes(node->child);
  if (node->next)
    draw_node_tree_bounding_boxes(node->next);
}

void render_editor(Ed *self, GfxState *gfx, Region *map, Game *game) {
  if (!self->open)
    return;

  Vector3 loc;

  bool hit =
      get_mouse_placement_loc(game, self->y * (float)(GLOBAL_SCALE), &loc);

  // loc = Vector3Transform(loc, MatrixRotateY(180.0f));

  // DrawCube(loc, 0.2f, 0.2f, 0.2f, GREEN);

  const int cs = CUBE_SIZE;
  Vector3 clamped =
      (Vector3){ceil(loc.x) - 0.5f, ceil(loc.y), ceil(loc.z) - 0.5f};
  clamped.y = self->y;
  // clamped.x -= 1;
  // clamped.z -= 1;

  if (IsKeyDown(KEY_LEFT_ALT))
    clamped = loc;

  DrawCylinder((Vector3){map->player_x, -3.0f, map->player_z}, 0.2f, 0.2f, 4.0f,
               20, (Color){0, 100, 255, 100});

  if (self->selected_node != NULL) {
    Node *node = self->selected_node;

    if (node->type == NODE_TYPE_MODEL) {
      Transform trans = node->transform;

      DrawModelWiresEx(node->model, Vector3Zero(), (Vector3){0, 0, 0}, 0.0f,
                       trans.scale, (Color){255, 0, 255, 200});
    }
  }

  draw_node_tree_bounding_boxes(map->scene_root);

  // Draw debug shapes
  for (int i = 0; i < map->num_spawns; i++) {
    Texture2D tex = game->assets->textures[TEX_GIRL_1 + self->model];

    ActorSpawn s = map->spawns[i];

    DrawCylinder((Vector3){s.position.x, -0.5f, s.position.z}, 0.0001f, 0.3f,
                 0.3f, 20, (Color){130, 50, 100, 120});

    DrawBillboardRec(*game->camera, tex,
                     (Rectangle){0, 0, tex.width, tex.height}, s.position, 1.0,
                     (Color){255, 255, 255, 130});

    // draw_billboard(gfx, s.position, tex,
    //                (Rectangle){0, 0, tex.width, tex.height}, 1.0f);
  }

  for (int light_i = 0; light_i < game->assets->num_lights; light_i++) {
    Light light = game->assets->lights[light_i];

    if (!light.enabled)
      continue;

    Color c = light.color;

    // Draw debug sphere where light is
    DrawSphere(light.position, 0.2f, (Color){c.r, c.g, c.b, 100});
  }

  // Draw debug exits

  for (int exit_i = 0; exit_i < map->num_exits; exit_i++) {
    Exit theExit = map->exits[exit_i];

    DrawCylinder((Vector3){theExit.position.x, -3.0f, theExit.position.z}, 0.2f,
                 0.2f, 4.0f, 20, (Color){255, 100, 0, 100});
  }

  float scale = 0.3f;
  float occ =
      (1.0f - scale) + ((1.0f + cosf((float)GetTime() * 10.0f)) * 0.5f) * scale;

  Model model = game->assets->models[0];
  int i = 0;
  const char *key;
  map_iter_t iter = map_iter(game->assets->models_dict);
  while ((key = map_next(game->assets->models_dict, &iter))) {
    model = *(*(Model **)map_get(game->assets->models_dict, key));
    if (i == self->model)
      break;

    i++;
  }

  if (self->object_placement_type == PLACE_NODES && self->light_grabbed < 0 &&
      self->state == EDITOR_STATE_NONE) {
    Transform transform;
    transform.translation = loc;
    transform.rotation = QuaternionIdentity();
    transform.scale = Vector3One();

    draw_model(gfx, &model, transform,
               (Color){(unsigned char)(occ * 255), 255,
                       (unsigned char)(occ * 255), 150});

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->lock_camera) {
      // Try to intersect
      Node *node = create_node_from_model(model, key);
      node->transform = transform;

      node_prepend(game->map->scene_root, node);

      self->object_placement_type = PLACE_NONE;
      self->selected_node = node;
    }

  } else if (self->object_placement_type == PLACE_ACTORS) {
    Texture2D tex = game->assets->textures[TEX_GIRL_1 + self->model];
    // DrawBillboard(*game->camera, tex, loc, CUBE_SIZE, WHITE);
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      DrawSphere((Vector3){loc.x, 0.0f, loc.z}, 0.01f, RED);
      DrawSphere((Vector3){loc.x, 0.0f, loc.z}, 0.02f,
                 (Color){255, 40, 0, 100});

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsMouseOnUiElement()) {
        float closest_dist = FLOAT_MAX;
        int index = -1;
        for (int i = 0; i < map->num_spawns; i++) {
          float dist = Vector3Distance(map->spawns[i].position, loc);
          if (dist < closest_dist) {
            closest_dist = dist;
            index = i;
          }
        }

        if (closest_dist < INT_MAX) {
          if (closest_dist < 1.0f) {
            for (int i = index; i < map->num_spawns - 1; i++) {
              map->spawns[i] = map->spawns[i + 1];
            }
            map->num_spawns--;
          }
        }
      }
    } else {
      draw_billboard(gfx, loc, tex, (Rectangle){0, 0, tex.width, tex.height},
                     1.0f);

      DrawCylinder((Vector3){loc.x, -0.5f, loc.z}, 0.0001f, 0.3f, 0.3f, 20,
                   (Color){130, 50, 100, 120});
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->lock_camera) {
        // assemble(ACTOR_GIRL_1 + self->model, game, loc.x, loc.y,
        // loc.z, 0,
        //  0);

        if (!add_actor_spawn(map, ACTOR_GIRL_1 + self->model, loc)) {
          push_message(self, "Max entity spawns placed for chunk");
        }
      }
    }

  } else if (self->object_placement_type == PLACE_PROPS) {
    Texture2D tex = game->assets->textures[TEX_PROPS];

    if (IsKeyDown(KEY_LEFT_SHIFT)) {
      DrawSphere((Vector3){loc.x, 0.0f, loc.z}, 0.01f, RED);
      DrawSphere((Vector3){loc.x, 0.0f, loc.z}, 0.02f,
                 (Color){255, 40, 0, 100});

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsMouseOnUiElement()) {
        float closest_dist = FLOAT_MAX;
        int index = -1;
        for (int i = 0; i < map->num_props; i++) {
          float dist = Vector3Distance(map->props[i].position, loc);
          if (dist < closest_dist) {
            closest_dist = dist;
            index = i;
          }
        }

        if (closest_dist < INT_MAX) {
          if (closest_dist < 1.0f) {
            for (int i = index; i < map->num_props - 1; i++) {
              map->props[i] = map->props[i + 1];
            }
            map->num_props--;
          }
        }
      }

    } else {
      Prop prop = prop_types[self->model];
      prop.position = loc;
      draw_prop(gfx, game, prop);

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsMouseOnUiElement()) {
        map->props[map->num_props++] = prop;
        printf("Added prop [%d]\n", map->num_props);
      }
    }

  } else if (self->object_placement_type == PLACE_MARKERS) {
    Color color = (Color){0, 100, 255, 100};

    if (self->which_marker == MARKER_EXIT)
      color = (Color){255, 100, 0, 100};

    DrawCylinder((Vector3){loc.x, -3.0f, loc.z}, 0.2f, 0.2f, 4.0f, 20, color);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !IsMouseOnUiElement()) {
      if (self->which_marker == MARKER_PLAYER_START) {
        map->player_x = loc.x;
        map->player_z = loc.z;
      } else if (self->which_marker == MARKER_EXIT) {
        self->state = EDITOR_STATE_EXIT_PLACEMENT_MODAL;
      }
    }
  }
}

#define MIN3(a, b, c)                                                          \
  ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2) {
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int *column = talloc(sizeof(unsigned int) * s1len + 1);
  for (y = 1; y <= s1len; y++)
    column[y] = y;
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

int sort_levenshtein(const void *a, const void *b) {
  char *aa = (char *)a;
  char *bb = (char *)b;
  int l1 = levenshtein(load_buff, aa);
  int l2 = levenshtein(load_buff, bb);
  if (l1 > l2)
    return 1;
  if (l1 < l2)
    return -1;
  if (l1 == l2)
    return 0;
  return 0;
}

void do_lights_modal(Ed *self, GfxState *gfx, Game *game, Region *map) {
  int id = 100;
  Shader *shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

  const float lx = PANEL_X;
  do_frame(lx, self->lights_panel_y + PANEL_BTN_HEIGHT, PANEL_WIDTH,
           GetScreenHeight(), 0.8f);

  float cursor_y = self->lights_panel_y;

  if (do_btn(lx + PANEL_WIDTH - PANEL_BTN_WIDTH, cursor_y, PANEL_BTN_WIDTH, 20,
             TextFormat(
                 "%s Lights",
                 ((self->state == EDITOR_STATE_LIGHTS_MODAL) ? "+" : "-")))) {
    if (self->state != EDITOR_STATE_LIGHTS_MODAL) {
      self->state = EDITOR_STATE_LIGHTS_MODAL;
    } else {
      self->state = EDITOR_STATE_NONE;
    }
  }

  if (self->state == EDITOR_STATE_LIGHTS_MODAL) {
    self->lights_panel_y =
        lerp(GetFrameTime() * 10.0f, self->lights_panel_y, PANEL_BTN_HEIGHT);
    // Do lights panel
    cursor_y += PANEL_BTN_HEIGHT;

    if (do_btn(lx + 2, cursor_y + 2, 30, 30, "+")) {
      Light *light = &game->assets->lights[game->assets->num_lights++];
      light->enabled = true;
      light->color = WHITE;
      UpdateLightValues(game->assets->shaders[SHADER_PHONG_LIGHTING], *light);
    }

    cursor_y += 32 + 2;

    static bool sun_header = 0;
    static bool lights_headers[MAX_LIGHTS] = {0};
    if (do_collapsing_header(&sun_header, "Sun", lx + 10, cursor_y,
                             PANEL_WIDTH - 20, 40)) {
      cursor_y += 40 + 2;

      do_label("Direction", lx + 30, cursor_y, 200, 30, 20);

      Vector3 test = {0};
      do_drag_float_3(&id, lx + 130, cursor_y, 300 - 64, 24,
                      &game->assets->sun.direction, 0.1f);
      id++;

      cursor_y += 32;

      do_label("Diffuse", lx + 30, cursor_y, 100, 30, 20);

      Color color = {255};
      do_color_drag_float_4(&id, lx + 30 + 100, cursor_y, 300 - 64, 24,
                            &game->assets->sun.diffuse);
      id++;

      cursor_y += 26;

      do_label("Ambient", lx + 30, cursor_y, 100, 30, 20);
      do_color_drag_float_4(&id, lx + 30 + 100, cursor_y, 300 - 64, 24,
                            &game->assets->sun.ambient);
      id++;

      UpdateSunValue(*shader, game->assets->sun);
    }

    cursor_y += 32 + 2;

    for (int i = 0; i < game->assets->num_lights; i++) {
      Light light = game->assets->lights[i];

      if (do_collapsing_header(&lights_headers[i],
                               TextFormat("%s[%d]", "Light", i), lx + 10,
                               cursor_y, PANEL_WIDTH - 20, 40)) {
        cursor_y += 40 + 2;

        do_label("Disabled", lx + 30, cursor_y, 100, 30, 20);
        do_check_box(&game->assets->lights[i].enabled, lx + 30 + 100, cursor_y,
                     30, 30);

        if (do_btn(lx + 30 + 130 + 10, cursor_y, 100, 30, "Grab")) {
          self->light_grabbed = i;
        }

        cursor_y += 32;
        do_label("Position", lx + 30, cursor_y, 200, 30, 20);

        do_drag_float_3(&id, lx + 130, cursor_y, 300 - 64, 24,
                        &game->assets->lights[i].position, 0.1f);

        cursor_y += 26;

        do_label("Color", lx + 30, cursor_y, 100, 30, 20);

        do_color_drag_float_4(&id, lx + 30 + 100, cursor_y, 300 - 64, 24,
                              &game->assets->lights[i].color);

        cursor_y += 28;

        UpdateLightValues(*shader, game->assets->lights[i]);

      } else {
        cursor_y += 40 + 2;
      }
    }

  } else {
    self->lights_panel_y = lerp(GetFrameTime() * 10.0f, self->lights_panel_y,
                                GetScreenHeight() - PANEL_BTN_HEIGHT);
  }

  int y = 0;
  for (int i = self->num_notes - 1; i >= 0; i--) {
    Note note = self->notes[i];

    if (!note.active)
      continue;

    Rectangle r = {(float)GetScreenWidth() - 200.0f,
                   (float)GetScreenHeight() - (NOTE_HEIGHT * (y + 1.0f)), 200,
                   NOTE_HEIGHT};
    do_frame(r.x - 4.0f, r.y - 4.0f - ((y + 1.0f) * 4.0f), r.width + 8.0f,
             r.height + 8.0f, 1.0f);
    do_label(note.mesg, r.x, r.y - ((y + 1.0f) * 4.0f), r.width, r.height, 20);
    y++;
  }
}

void do_node_tree_modal(Ed *self, GfxState *gfx, Game *game, Region *map) {
  const float lx = PANEL_X;

  do_frame(lx, self->node_tree_panel_y + PANEL_BTN_HEIGHT, PANEL_WIDTH,
           GetScreenHeight(), 0.8f);

  float cursor_y = self->node_tree_panel_y;

  if (do_btn(
          lx + PANEL_WIDTH - PANEL_BTN_WIDTH * 3, cursor_y, PANEL_BTN_WIDTH, 20,
          TextFormat(
              "%s Scene",
              ((self->state == EDITOR_STATE_NODE_TREE_MODAL) ? "+" : "-")))) {
    toggle_model_node_tree_modal(self);
  }

  if (self->state == EDITOR_STATE_NODE_TREE_MODAL) {
    self->node_tree_panel_y =
        lerp(GetFrameTime() * 10.0f, self->node_tree_panel_y, PANEL_BTN_HEIGHT);
    cursor_y += PANEL_BTN_HEIGHT;

    // Start drawing the panel
    {
      Node *child = map->scene_root;
      Node *next = NULL;

      float cx = lx + MARGIN;
      float cy = cursor_y + MARGIN;

      int id = 500;

      while (child) {
        id++;
        do_node_dropdown(self, child, &id, &cx, &cy);

        cx += 8;
        cy += 20;

        if (child->next) {
          next = child->next;
          while (next) {
            id++;
            do_node_dropdown(self, next, &id, &cx, &cy);
            cy += 20;
            next = next->next;
          }
        }

        child = child->child;
      }
    }
  } else {
    self->node_tree_panel_y =
        lerp(GetFrameTime() * 10.0f, self->node_tree_panel_y,
             GetScreenHeight() - PANEL_BTN_HEIGHT);
  }
}

void do_node_dropdown(Ed *self, Node *node, int *id, float *cx, float *cy) {
  static bool active = true;

  const char *label = (node->type == NODE_TYPE_MODEL ? node->name : "Empty");

  const float w = PANEL_WIDTH - (MARGIN * 8);
  const float x = *cx;
  const float y = *cy;

  const float label_w = 60;

  if (do_collapsing_header(&active, label, (*cx), (*cy), w, 20)) {
    (*cy) += 20;
    do_label("pos: ", (*cx), (*cy), label_w, 20, 20);
    do_drag_float_3(id, (*cx) + label_w, (*cy), w, 20,
                    &node->transform.translation, 0.1f);
    (*id) += 1;
    (*cy) += 20;
    do_label("scale: ", (*cx), (*cy), label_w, 20, 20);
    do_drag_float_3(id, (*cx) + label_w, (*cy), w, 20, &node->transform.scale,
                    0.1f);

    (*id) += 1;
    (*cy) += 20;

    Vector3 rot = QuaternionToEuler(node->transform.rotation);
    do_label("rot: ", (*cx), (*cy), label_w, 20, 20);
    do_drag_float_3(id, (*cx) + label_w, (*cy), w, 20, &rot, 0.01f);
    node->transform.rotation = QuaternionFromEuler(rot.x, rot.y, rot.z);
  }
}

void do_models_modal(Ed *self, GfxState *gfx, Game *game, Region *map) {
  int id = 200;
  const float lx = PANEL_X;

  do_frame(lx, self->models_panel_y + PANEL_BTN_HEIGHT, PANEL_WIDTH,
           GetScreenHeight(), 0.8f);

  float cursor_y = self->models_panel_y;

  if (do_btn(lx + PANEL_WIDTH - PANEL_BTN_WIDTH * 2, cursor_y, PANEL_BTN_WIDTH,
             PANEL_BTN_HEIGHT,
             TextFormat("%s Models",
                        ((self->state == EDITOR_STATE_MODEL_SELECTOR_MODAL)
                             ? "+"
                             : "-")))) {
    toggle_model_selector_modal(self);

    if (self->state == EDITOR_STATE_MODEL_SELECTOR_MODAL) {
      self->object_placement_type = PLACE_NODES;
    }
  }

  if (self->state == EDITOR_STATE_MODEL_SELECTOR_MODAL) {
    self->models_panel_y =
        lerp(GetFrameTime() * 10.0f, self->models_panel_y, PANEL_BTN_HEIGHT);

    cursor_y += PANEL_BTN_HEIGHT;

    static float scroll_y = 0;
    static float max_height = 300;
    begin_scroll_panel_v(id++, lx + 1, cursor_y + 1, PANEL_WIDTH + 18 - 1,
                         GetScreenHeight() - 1, &scroll_y, max_height);

    cursor_y += 30;

    Rectangle where = {lx + MARGIN, cursor_y, 100, 100};

    max_height = 0;

    const char *key;
    map_iter_t iter = map_iter(game->assets->models_dict);

    int i = 0;
    while ((key = map_next(game->assets->models_dict, &iter))) {
      const float size = PANEL_WIDTH / 2;
      Model model = *(*(Model **)map_get(game->assets->models_dict, key));

      Texture2D texture = model.materials[0].maps[MAP_DIFFUSE].texture;

      Vector3 cpos = game->camera->position;

      // DrawTexturePro(
      //     texture, (Rectangle){0, 0, texture.width,
      //     texture.height}, (Rectangle){lx + 30, cursor_y -
      //     scroll_y, size, size}, Vector2Zero(), 0.0f, WHITE);

      if (do_tex_btn(lx + 10, cursor_y - scroll_y, size, size, "", texture)) {
        self->model = i;
        self->state = EDITOR_STATE_NONE;
        game->lock_camera = false;
      }

      Rectangle where = {lx + 80, cursor_y - scroll_y + 70, size - 140,
                         size - 140};

      {
        // Model model = game->assets->models[0];
        Model model = *(*(Model **)map_get(game->assets->models_dict, key));

        draw_gui_model(gfx, &model, where, (Vector3){0, GetTime(), 0}, RED);

        if (self->model == i) {
          DrawRectangleLinesEx(
              (Rectangle){lx + 10, cursor_y - scroll_y, size, size}, 2, RED);
        }
      }

      cursor_y += size + 8;
      max_height += size + 8;
      i++;
    }

    id += map->num_models + 10;

    end_scroll_panel_v();
  } else {
    self->models_panel_y = lerp(GetFrameTime() * 10.0f, self->models_panel_y,
                                GetScreenHeight() - PANEL_BTN_HEIGHT);
  }
}

void do_selected_node_panel(Ed *self, GfxState *gfx, Game *game, Region *map) {
  int x = GetScreenWidth() / 2 - 600;
  int y = 32;
  int w = 300;
  int h = GetScreenHeight() - 60;
  do_panel(x, y, w, h);

  int cursor_y = y + 20;

  const float label_w = 60;

  do_label("rot: ", x + 20, cursor_y, label_w, 20, 20);

  Vector3 rot = QuaternionToEuler(self->selected_node->transform.rotation);
  int id = 700;
  do_drag_float_3(&id, x + 20 + label_w, cursor_y, 300 - (label_w - 20), 20,
                  &rot, 0.1f);
}

void render_editor_ui(Ed *self, GfxState *gfx, Region *map, Game *game) {
  Shader *shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

  int id = 10;

  render_console(self, map, game, id);

  id += 50;

  if (!self->open) {
    if (do_btn(GetScreenWidth() / 2 - 50.0f, 0, 100, 25, "Editor")) {
      push_message(self, "Editor mode!");
      self->open = true;
    }
    return;
  }

  {
    const int x = GetScreenWidth() / 2 + 50.0f + 8.0f;
    const int y = 0;
    const int w = 50;
    const int h = 25;

    if (do_btn(x, y, w, h, "")) {
      self->open = false;
    }

    DrawTriangle((Vector2){x + 12, y + 4}, (Vector2){x + 12, y + h - 4},
                 (Vector2){x + w - 12, y + ((h / 2))}, GREEN);
  }

  if (do_btn(GetScreenWidth() / 2 - 50.0f, 0, 100, 25, "Play")) {
    self->open = false;
  }

  // Notifications
  int last = self->object_placement_type;
  do_toggle_group_v(&self->object_placement_type,
                    "NONE|NODES|ACTORS|PROPS|MARKERS|", 0,
                    GetScreenHeight() - (self->placement_toggle_height + 50),
                    &self->placement_toggle_height);

  // Draws a label for the type of marker
  // TODO(Dustin): Add new markers for exit and other stuff
  if (self->object_placement_type == PLACE_MARKERS) {
    if (self->which_marker == MARKER_PLAYER_START) {
      do_center_x_label((float)GetScreenWidth(),
                        (float)GetScreenHeight() / 2 + 50.0f, 30,
                        "Player start");
    } else if (self->which_marker == MARKER_EXIT) {
      do_center_x_label((float)GetScreenWidth(),
                        (float)GetScreenHeight() / 2 + 50.0f, 30, "Place exit");
    }
  }

  // Region loading and unloading
  if (do_btn(100, GetScreenHeight() - 30.0f, 100, 30, "Export")) {
    // self->do_export_modal = true;
    self->state = EDITOR_STATE_EXPORT_MODAL;
  }

  if (do_btn(0, GetScreenHeight() - 30.0f, 100, 30, "Load")) {
    self->state = EDITOR_STATE_LOAD_MODAL;
  }

  if (self->state == EDITOR_STATE_LOAD_MODAL) {
    do_modal();
    Unlock();

    // TODO(Dustin): Get this to work
    // qsort(self->maps, self->num_maps, sizeof(char*),
    // sort_levenshtein);

    static int load_input_cursor = 0;
    if (do_text_input(load_buff, &load_input_cursor, 1024,
                      GetScreenWidth() / 2 - 150.f,
                      GetScreenHeight() / 2 - 25.f, 300, 50)) {
      tstr path = talloc(512);
      sprintf(path, "resources/maps/%s.janet", load_buff);

      if (FileExists(path) == false) {
        tstr msg = talloc(512);
        sprintf(msg, "File [%s] does not exist", path);

        push_message(self, msg);
      } else {
        reset_region_to_zero(game->map, game);
        load_region_from_script(game->map, path, game);
      }

      self->state = EDITOR_STATE_NONE;
    }

    {
      static float scroll_y = 0;
      static float max_height = 0;
      begin_scroll_panel_v(id++, GetScreenWidth() / 2 - 150.0f,
                           GetScreenHeight() / 2 + 28, 300 + 12, 300, &scroll_y,
                           max_height);

      max_height = 0;
      for (int i = 2; i < self->num_maps; i++) {
        if (do_btn(GetScreenWidth() / 2 - 150.0f,
                   GetScreenHeight() / 2 + 20 + 30 * (i - 1.0f) - scroll_y, 300,
                   30, self->maps[i])) {
          const char *path = TextFormat("resources/maps/%s", self->maps[i]);
          if (FileExists(path)) {
            reset_region_to_zero(game->map, game);
            load_region_from_script(game->map, path, game);
            self->state = EDITOR_STATE_NONE;
          }
        }
        max_height += 30;
      }

      end_scroll_panel_v();
    }

    if (do_btn(GetScreenWidth() / 2 + 200.0f, GetScreenHeight() / 2.0f, 30, 30,
               "X")) {
      self->state = EDITOR_STATE_NONE;
    }

    Lock();
  } else if (self->state == EDITOR_STATE_EXPORT_MODAL) {
    do_modal();
    Unlock();

    do_center_x_label((float)GetScreenWidth(),
                      (float)GetScreenHeight() / 2 - 100, 30, "Region Name");

    static char buffer[100] = {'\0'};

    static int export_input_cursor = 0;
    if (do_text_input(buffer, &export_input_cursor, 100,
                      GetScreenWidth() / 2 - 150.0f, GetScreenHeight() / 2, 300,
                      50)) {
      tstr path = talloc(512);
      sprintf(path, "resources/maps/%s.janet", buffer);
      serialize_map(self, map, game, path);
      push_message(self, FormatText("Exported [%s]", path));
      self->state = EDITOR_STATE_NONE;
    }

    {
      static float scroll_y = 0;
      static float max_height = 0;
      begin_scroll_panel_v(id++, GetScreenWidth() / 2 - 150.0f,
                           GetScreenHeight() / 2 + 20 + 30 * 2, 300 + 12, 300,
                           &scroll_y, max_height);

      max_height = 0;
      for (int i = 2; i < self->num_maps; i++) {
        if (do_btn(GetScreenWidth() / 2 - 150.0f,
                   GetScreenHeight() / 2 + 20 + 30 * (i - 1.0f) - scroll_y, 300,
                   30, self->maps[i])) {
          const char *path = TextFormat("resources/maps/%s", self->maps[i]);
          if (FileExists(path)) {
            // reset_region_to_zero(game->map, game);
            // load_region_from_script(game->map, path, game);
            serialize_map(self, map, game, path);
            self->state = EDITOR_STATE_NONE;
          }
        }
        max_height += 30;
      }

      end_scroll_panel_v();
    }

    if (do_btn(GetScreenWidth() / 2.f + 200, GetScreenHeight() / 2.0f, 30, 30,
               "X")) {
      self->state = EDITOR_STATE_NONE;
    }

    Lock();

  } else if (self->state == EDITOR_STATE_EXIT_PLACEMENT_MODAL) {
    do_modal();
    Unlock();

    game->lock_camera = true;

    const char *title = "Dest map name";
    Vector2 size = MeasureTextEx(GetFont(), title, 30, 1);

    static char dest_buffer[512] = {'\0'};

    float cursor_y = GetScreenHeight() / 2 - 200;
    do_label(title, GetScreenWidth() / 2 - size.x - 6, cursor_y, size.x + 20,
             size.y + 4, 30);

    static dest_input_cursor = 0;
    do_text_input(dest_buffer, &dest_input_cursor, 512,
                  GetScreenWidth() / 2 + 6, cursor_y, 200, size.y + 4);

    cursor_y += size.y + 4 + 8;

    const char *title_2 = "This door id";
    size = MeasureTextEx(GetFont(), title_2, 30, 1);
    do_label(title_2, GetScreenWidth() / 2 - size.x - 6, cursor_y, size.x + 20,
             size.y + 4, 30);

    static int door_id = 0;
    do_incrementer(GetScreenWidth() / 2 + 6, cursor_y, size.y + 4, size.y + 4,
                   &door_id, 20);

    cursor_y += size.y + 4 + 8;

    const char *title_3 = "Dest door id";
    size = MeasureTextEx(GetFont(), title_3, 30, 1);
    do_label(title_3, GetScreenWidth() / 2 - size.x - 6, cursor_y, size.x + 20,
             size.y + 4, 30);

    static int dest_door_id = 0;
    do_incrementer(GetScreenWidth() / 2 + 6, cursor_y, size.y + 4, size.y + 4,
                   &dest_door_id, 20);

    cursor_y += size.y + 4 + 8;

    if (do_btn(GetScreenWidth() / 2 + 200, GetScreenHeight() / 2, 30, 30,
               "X")) {
      self->state = EDITOR_STATE_NONE;
    }

    if (do_btn(GetScreenWidth() / 2 - 100, cursor_y, 200, 30, "Place")) {
      Vector3 loc = {0};
      bool hit =
          get_mouse_placement_loc(game, self->y * (int)(GLOBAL_SCALE), &loc);
      add_exit(map, loc, door_id, dest_door_id, dest_buffer);

      self->state = EDITOR_STATE_NONE;
    }

    Lock();
  } else {
    Unlock();
  }

  if (last != self->object_placement_type) {
    self->model = 0;
  }

  do_lights_modal(self, gfx, game, map);
  do_models_modal(self, gfx, game, map);
  do_node_tree_modal(self, gfx, game, map);

  if (self->do_selected_node_panel) {
    do_selected_node_panel(self, gfx, game, map);
  }
}

void dump_transform(StringBuilder *sb, Transform transform) {
  sb_appendf(sb, ":transform @[%f %f %f  %f %f %f  %f %f %f %f]",
             transform.translation.x, transform.translation.y,
             transform.translation.z, transform.scale.x, transform.scale.y,
             transform.scale.z, transform.rotation.x, transform.rotation.y,
             transform.rotation.z, transform.rotation.w);
}

void dump_node(StringBuilder *sb, Node *node) {
  sb_append(sb, "@{");

  if (node->type == NODE_TYPE_EMPTY) {
  } else if (node->type == NODE_TYPE_MODEL) {
    sb_appendf(sb, " :model \"%s\"", node->name);
  } else if (node->type == NODE_TYPE_BILLBOARD) {
    assert(0);
  }

  // Transform
  sb_append(sb, "\n");
  dump_transform(sb, node->transform);

  if (node->next) {
    sb_append(sb, "\n:next ");
    dump_node(sb, node->next);
  }

  if (node->child) {
    sb_append(sb, "\n:child ");
    dump_node(sb, node->child);
  }

  sb_append(sb, "}");
}

void serialize_map(Ed *editor, Region *map, Game *game, const char *path) {
  StringBuilder *sb = sb_create();
  char *result;

  sb_appendf(
      sb,
      "@{ :size @[%d %d]\n   :start @[%f %f]\n   :sun-direction @[%f %f %f]\n",
      map->width, map->height, map->player_x, map->player_z,
      game->assets->sun.direction.x, game->assets->sun.direction.y,
      game->assets->sun.direction.z);

  sb_append(sb, "    :layers @[");

  for (int layer = 0; layer < MAX_NUM_LAYERS; layer++) {
    sb_append(sb, "\n      @{ :data ``");

    for (int y = 0; y < map->height; y++) {
      for (int x = 0; x < map->width; x++) {
        Wall wall = map->walls[layer][x + y * map->width];
        if (wall.active) {
          sb_appendf(sb, "%d", wall.model + 1);
        } else {
          sb_append(sb, ".");
        }
      }
    }
    sb_append(sb, "`` }");
  }

  sb_append(sb, "]\n   :props @[");

  for (int prop = 0; prop < map->num_props; prop++) {
    Prop p = map->props[prop];
    sb_appendf(sb, "@[%f %f %f %f %f %f %f %f]\n", p.region.x, p.region.y,
               p.region.width, p.region.height, p.position.x, p.position.y,
               p.position.z, p.scale);
  }

  sb_append(sb, "]\n   :lights @[");

  for (int i = 0; i < game->assets->num_lights; i++) {
    Light light = game->assets->lights[i];

    sb_appendf(sb, "\n      @[%s  %f %f %f  %d %d %d]",
               (light.enabled ? "true" : "false"), light.position.x,
               light.position.y, light.position.z, light.color.r, light.color.g,
               light.color.b);
  }

  sb_append(sb, "]\n   :actors @[");

  for (int i = 0; i < map->num_spawns; i++) {
    ActorSpawn s = map->spawns[i];
    sb_appendf(sb, "\n      @[%d  %f %f %f]", s.type - ACTOR_GIRL_1,
               s.position.x, s.position.y, s.position.z);
  }

  sb_append(sb, "]\n   :exits @[");

  for (int i = 0; i < map->num_exits; i++) {
    Exit e = map->exits[i];
    //                           id   x  y  z  destId destPath
    sb_appendf(sb, "\n      @[%d  %f %f %f  %d \"%s\"]", e.id, e.position.x,
               e.position.y, e.position.z, e.dest_id, e.dest_path);
  }

  sb_append(sb, "]\n   :root-node ");

  if (map->scene_root != NULL)
    dump_node(sb, map->scene_root);
  else
    sb_append(sb, "@{}");

  sb_append(sb, "}");

  result = sb_concat(sb);

  FILE *f = fopen(path, "w");
  fwrite(result, sizeof(char), strlen(result), f);
  fclose(f);

  free(result);
  sb_free(sb);
}

void unserialize_map(Ed *editor, Region *map, Game *game, const char *path) {}
