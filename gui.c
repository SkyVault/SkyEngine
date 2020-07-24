#include "gui.h"

#define MARGIN (8)

#define CHECK_IF_HOT()                                                         \
  Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};          \
  state->hot = CheckCollisionPointRec(GetMousePosition(), rect);               \
  if (state->hot)                                                              \
    GuiState.a_el_is_hot = true;

#define GET_STATE() struct NState *state = &GuiState.states[id];
#define IS_HOT()                                                               \
  CheckCollisionPointRec(                                                      \
      GetMousePosition(),                                                      \
      (Rectangle){x + GuiState.px, y + GuiState.py, width, height});

void reset_gui() {
  for (int i = 0; i < MAX_NUM_STATES; i++) {
    GuiState.states[i] = (struct NState){0};
  }
}

void init_gui() {
  // GuiState.font = LoadFont("resources/HappyTime.otf");
  GuiState.font = GetFontDefault();
  GuiState.a_el_is_hot = false;
}

void do_panel(float x, float y, float width, float height) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;

  Color color = (!hot) ? BASE_COLOR : (Color){200, 200, 200, 255};

  DrawRectangle(x, y, width, height, color);
  DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3, ALT_COLOR);
}

void do_frame(float x, float y, float width, float height, float alpha) {
  Color color = (Color){100, 100, 100, (unsigned char)(255 * alpha)};

  DrawRectangle(x, y, width, height, color);
  DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3,
                       (Color){200, 200, 200, 200});
}

void do_window(WindowState *state, const char *title) {
  Vector2 title_size = MeasureTextEx(GuiState.font, title, 20, 1);
  do_frame(state->region.x, state->region.y, state->region.width,
           state->region.height, 0.9f);
  do_frame(state->region.x, state->region.y - 20, title_size.x + MARGIN * 2, 20,
           0.9f);

  do_label(title, state->region.x + MARGIN, state->region.y - 20, title_size.x,
           20, 20);

  bool hot = false;
  bool title_bar_hot = false;
  if (CheckCollisionPointRec(GetMousePosition(), state->region)) {
    GuiState.a_el_is_hot = true;
    hot = true;
  }

  if (CheckCollisionPointRec(GetMousePosition(),
                             (Rectangle){.x = state->region.x,
                                         .y = state->region.y - 20,
                                         .width = title_size.x,
                                         .height = 20})) {
    GuiState.a_el_is_hot = true;
    hot = true;
    title_bar_hot = true;
  }

  if (hot) {
    if (title_bar_hot) {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state->last_mouse = GetMousePosition();
      } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 delta = Vector2Subtract(GetMousePosition(), state->last_mouse);
        state->region.x += delta.x;
        state->region.y += delta.y;

        state->last_mouse = GetMousePosition();
      }
    } else if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
      state->last_mouse = GetMousePosition();
    } else if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
      Vector2 delta = Vector2Subtract(GetMousePosition(), state->last_mouse);
      state->region.x += delta.x;
      state->region.y += delta.y;

      state->last_mouse = GetMousePosition();
    }
  }
}

void do_label(const char *str, float x, float y, float width, float height,
              int font_size) {
  DrawTextRec(GuiState.font, str, (Rectangle){x, y, width, height}, font_size,
              1, true, HIGHLIGHT_COLOR);
}

bool do_click_region(float x, float y, float width, float height) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;
  return hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool do_btn(float x, float y, float width, float height, const char *text) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;
  bool active = hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  const float size = height - 2;

  Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);

  // Draw the button
  do_panel(x, y, width, height);
  DrawTextEx(GuiState.font, text,
             (Vector2){
                 x + width / 2 - measure.x / 2,
                 y + height / 2 - measure.y / 2,
             },
             size, 1, HIGHLIGHT_COLOR);

  return active;
}

bool do_tex_btn(float x, float y, float width, float height, const char *text,
                Texture2D texture) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;
  bool active = hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  const float size = 30.0f;

  Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);

  // Draw the button
  do_panel(x, y, width, height);

  Color color = {200, 200, 200, 255};
  if (hot)
    color = WHITE;

  DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height},
                 (Rectangle){x + 1, y + 1, width - 2, height - 2},
                 (Vector2){0.0f, 0.0f}, 0.0f, color);

  DrawTextEx(GuiState.font, text,
             (Vector2){
                 x + width / 2 - measure.x / 2,
                 y + height / 2 - measure.y / 2,
             },
             size, 1, HIGHLIGHT_COLOR);

  return active;
}

void do_center_x_label(float outer_width, float y, int font_size,
                       const char *text) {
  Vector2 measure = MeasureTextEx(GuiState.font, text, font_size, 1);

  DrawTextEx(GuiState.font, text,
             (Vector2){
                 outer_width / 2 - measure.x / 2,
                 y,
             },
             font_size, 1, HIGHLIGHT_COLOR);
}

void do_modal() {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                (Color){0, 0, 0, 200});
}

bool do_text_input(char *buffer, int *cursor, size_t buffer_size, float x,
                   float y, float width, float height) {
  do_panel(x, y, width, height);

  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;
  bool active = hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  const int fsize = 30;
  Vector2 size = MeasureTextEx(GuiState.font, buffer, fsize, 1);
  DrawTextEx(GuiState.font, buffer,
             (Vector2){x + 4, y + height / 2 - size.y / 2}, fsize, 1, BLACK);

  if (hot) {
    if (IsKeyDown(KEY_BACKSPACE)) {
      if (*cursor > 0) {
        buffer[(*cursor) - 1] = '\0';
        (*cursor) -= 1;
      }
    }

    int key = GetKeyPressed();
    if (key > 0) {
      buffer[*cursor] = (char)key;
      (*cursor) += 1;
    }
  }

  if (hot && (((int)(GetTime() * 3.0f) % 2) == 0)) {
    DrawRectangle(x + 4 + size.x, y + 4, 4, height - 8, BLACK);
  }

  return hot && IsKeyPressed(KEY_ENTER);
}

void do_toggle_group_v(int *which_, const char *names, float x, float y,
                       float *out_width) {
  char *it = names;
  char *end = it + strlen(names);

  float cursor_y = 0;
  const float r = 10;

  int which = 0;

  while (it != end && *it != '\0') {
    char *start = it;

    while (it != end && &it != '\0') {
      if (*it == '|') {
        tstr name = talloc(it - start + 1);
        sprintf(name, "%.*s", (int)(it - start), start);

        Vector2 size = MeasureTextEx(GuiState.font, name, 20, 1);

        bool hot = CheckCollisionPointRec(
            GetMousePosition(),
            (Rectangle){x, y + cursor_y, size.x + r * 2 + 6, size.y + 4});

        if (hot)
          GuiState.a_el_is_hot = true;

        do_panel(x, y + cursor_y, size.x + r * 2 + 6, size.y + 4);
        DrawTextEx(GuiState.font, name,
                   (Vector2){x + r * 2 + 2, y + cursor_y + 2}, 20, 1,
                   HIGHLIGHT_COLOR);

        DrawCircle(x + r + 2, y + 2 + cursor_y + size.y / 2, r, BASE_COLOR);
        DrawCircleLines(x + r + 2, y + 2 + cursor_y + size.y / 2, r, ALT_COLOR);

        if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          (*which_) = which;
        }

        if (which == (*which_)) {
          DrawCircle(x + r + 2.0f, y + 2 + cursor_y + size.y / 2.0f, r * 0.8f,
                     ALT_COLOR);
        }

        cursor_y += size.y + 4;
        which++;

        break;
      }
      it++;
    }

    it++;
  }

  if (out_width != NULL)
    *out_width = cursor_y;
}

bool do_check_box(bool *active, float x, float y, float width, float height) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;

  if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    (*active) = !(*active);
  }

  // Draw the button
  do_panel(x, y, width, height);

  if (*active)
    DrawRectangle((int)x + 1, (int)y + 1, (int)width - 2, (int)height - 2,
                  ALT_COLOR);

  return (*active);
}

bool do_collapsing_header(bool *active, const char *label, float x, float y,
                          float width, float height) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;

  if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    (*active) = !(*active);
  }

  char buff[512];
  sprintf(buff, "%s %s", ((*active) ? "-" : "+"), label);

  const Vector2 size = MeasureTextEx(GuiState.font, buff, height - 2, 1.0f);

  // Draw the button
  do_panel(x, y, width, height);
  do_label(buff, x + width / 2 - size.x / 2, y + 1, width, height, height - 2);

  return (*active);
}

bool do_drag_region_float(NodeId id, float x, float y, float width,
                          float height, float *value, float step) {
  struct NState *state = &GuiState.states[id];

  CHECK_IF_HOT()

  bool hot = IS_HOT();

  if (hot && IsKeyPressed(KEY_ZERO)) {
    *value = 0.0f;
    return true;
  }


  bool last = state->active;
  state->active = state->hot && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

  if (!last && state->active) {
    state->last_value = (float)GetMouseX();
  }

  if (last && state->active) {
    *value -= (state->last_value - (float)GetMouseX()) * step;
    state->last_value = (float)GetMouseX();
  }
}

bool do_drag_float(NodeId id, float x, float y, float width, float height,
                   float *value, float step) {
  struct NState *state = &GuiState.states[id];

  do_drag_region_float(id, x, y, width, height, value, step);

  const char *str = TextFormat("%.2f", *value);

  Vector2 size = MeasureTextEx(GuiState.font, str, 20, 1);

  do_panel(x, y, width, height);
  DrawTextRec(GuiState.font, str,
              (Rectangle){x + width / 2 - size.x / 2,
                          y + height / 2 - size.y / 2, width, height},
              20, 1, true, HIGHLIGHT_COLOR);

  return Active(id);
}

bool do_drag_float_3(NodeId *id, float x, float y, float width, float height,
                     Vector3 *value, float step) {
  int nid = *id;
  do_drag_float(nid++, x + width / 3 * 0, y, width / 3, height, &value->x,
                step);
  do_drag_float(nid++, x + width / 3 * 1, y, width / 3, height, &value->y,
                step);
  do_drag_float(nid++, x + width / 3 * 2, y, width / 3, height, &value->z,
                step); 
  *id = nid;

  return IS_HOT();
}

bool do_color_drag_float_4(NodeId *id, float x, float y, float width,
                           float height, Color *color) {
  struct NState *state = &GuiState.states[(*id)];

  if (state->init == 0) {
    state->init = 1;

    state->v = (Vector4){color->r / 255.0f, color->g / 255.0f,
                         color->b / 255.0f, color->a / 255.0f};
  }

  DrawRectangle(x, y, height, height, *color);

  float delta = 0;
  do_drag_region_float(*id, x, y, height, height, &delta, (1.0f / 255.0f));

  if (delta > 0 || delta < 0) {
    state->v.x += delta;
    state->v.y += delta;
    state->v.z += delta;
  }

  (*id)++;

  do_drag_float_3(id, x + height, y, width - height, height, &state->v,
                  (1.0 / 255.0));

  (*id)++;

  // Clamp
  state->v.x =
      (state->v.x < 0.0f ? 0.0f : (state->v.x > 1.0f ? 1.0f : state->v.x));

  state->v.y =
      (state->v.y < 0.0f ? 0.0f : (state->v.y > 1.0f ? 1.0f : state->v.y));

  state->v.z =
      (state->v.z < 0.0f ? 0.0f : (state->v.z > 1.0f ? 1.0f : state->v.z));

  color->r = (unsigned char)(state->v.x * 255);
  color->g = (unsigned char)(state->v.y * 255);
  color->b = (unsigned char)(state->v.z * 255);
  return true;
}

int do_incrementer(float x, float y, float width, float height, int *v,
                   float font_size) {
  bool hot = IS_HOT();
  if (hot)
    GuiState.a_el_is_hot = true;
  bool active = hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  const float size = font_size;

  const char *text = FormatText("%d", *v);

  Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);
  do_panel(x, y, width, height);
  DrawTextEx(GuiState.font, text,
             (Vector2){
                 x + width / 2 - measure.x / 2,
                 y + height / 2 - measure.y / 2,
             },
             size, 1, HIGHLIGHT_COLOR);

  if (hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    *v = (*v) + 1;
    return 1;
  } else if (hot && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    *v = (*v) - 1;
    return -1;
  }

  return 0;
}

void begin_scroll_panel_v(NodeId id, float x, float y, float width,
                          float height, float *scroll, float max_height) {
  const float scroll_w = 12;

  GET_STATE()
  CHECK_IF_HOT()

  if (state->hot && scroll != NULL) {
    int mdelta = GetMouseWheelMove();
    (*scroll) -= mdelta * 40.0f;
  }

  if (*scroll < 0)
    *scroll = 0.0f;
  if (*scroll > max_height)
    *scroll = max_height;

  float ratio = (height / max_height);
  float scroll_ratio = ((*scroll < 0.0f ? -(*scroll) : (*scroll)) / max_height);

  do_frame(x, y, width, height, 0.8f);

  DrawRectangleLines(x + width - scroll_w, y, scroll_w, height,
                     HIGHLIGHT_COLOR);

  const float yy = y + height * scroll_ratio;
  // TODO(Dustin): Allow dragging the scrollbar
  DrawRectangle(x + width - scroll_w, yy, scroll_w, (height / 2 * ratio),
                HIGHLIGHT_COLOR);

  BeginScissorMode(x, y, width - scroll_w, height);
}

void end_scroll_panel_v() { EndScissorMode(); }

Vector4 CTV4(Color c) {
  return (Vector4){
      c.r / 255.0f,
      c.g / 255.0f,
      c.b / 255.0f,
      c.a / 255.0f,
  };
}

Color V4TC(Vector4 v) {
  return (Color){
      (unsigned char)(v.x * 255),
      (unsigned char)(v.y * 255),
      (unsigned char)(v.z * 255),
      (unsigned char)(v.w * 255),
  };
}

void update_gui() { GuiState.a_el_is_hot = false; }

bool IsMouseOnUiElement() { return GuiState.a_el_is_hot; }

Font GetFont() { return GuiState.font; }

void Lock() { GuiState.locked = true; }

void Unlock() { GuiState.locked = false; }

bool Active(NodeId id) {
  return GuiState.states[id].active && !GuiState.locked;
}

bool Hot(NodeId id) { return GuiState.states[id].hot && !GuiState.locked; }
