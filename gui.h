#ifndef BENIS_GUI_H
#define BENIS_GUI_H

#include <stdbool.h>

#include "raylib.h"
#include "raymath.h"
#include "tween.h"
#include "utils.h"

typedef int NodeId;

#define BASE_COLOR                                                             \
  (Color) { 100, 100, 100, 200 }

#define HIGHLIGHT_COLOR                                                        \
  (Color) { 240, 240, 210, 200 }

#define ALT_COLOR                                                              \
  (Color) { 120, 120, 120, 200 }

struct NState {
  uint8_t active;
  uint8_t hot;
  uint8_t init;
  int cursor;
  float value;
  float last_value;
  Vector4 v;
};

#define MAX_NUM_STATES (1000)

static struct {
  struct NState states[MAX_NUM_STATES];
  float px, py;
  Font font;

  bool locked;
  bool a_el_is_hot;
} GuiState = {.px = 0.0f, .py = 0.0f, .locked = false};

typedef struct {
  Rectangle region;
  Vector2 last_mouse;
  int flags;
} WindowState;

void init_gui();
void reset_gui();
void update_gui();

void do_panel(float x, float y, float width, float height);
void do_frame(float x, float y, float width, float height, float alpha);
void do_window(WindowState *state, const char *title);
bool do_btn(float x, float y, float width, float height, const char *text);

bool do_tex_btn(float x, float y, float width, float height, const char *text,
                Texture2D texture);

bool do_click_region(float x, float y, float width, float height);
void do_center_x_label(float outer_width, float y, int font_size,
                       const char *text);

void do_label(const char *str, float x, float y, float width, float height,
              int font_size);

void do_modal();
bool do_text_input(char *buffer, int *cursor, size_t buffer_size, float x,
                   float y, float width, float height);

void do_toggle_group_v(int *which, const char *names, float x, float y,
                       float *out_width);

bool do_check_box(bool *active, float x, float y, float width, float height);
bool do_collapsing_header(bool *active, const char *label, float x, float y,
                          float width, float height);

Vector4 CTV4(Color c);
Color V4TC(Vector4 v);

bool do_drag_float(NodeId id, float x, float y, float width, float height,
                   float *value, float step);

bool do_drag_float_3(NodeId *id, float x, float y, float width, float height,
                     Vector3 *value, float step);

bool do_color_drag_float_4(NodeId *id, float x, float y, float width,
                           float height, Color *color);

int do_incrementer(float x, float y, float width, float height, int *v,
                   float font_size);

void begin_scroll_panel_v(NodeId id, float x, float y, float width,
                          float height, float *scroll, float max_height);

void end_scroll_panel_v();

bool IsMouseOnUiElement();

Font GetFont();

void Lock();
void Unlock();

bool Hot(NodeId id);
bool Active(NodeId id);

#endif // BENIS_GUI_H
