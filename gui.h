#ifndef BENIS_GUI_H
#define BENIS_GUI_H

#include <stdbool.h>

#include "raylib.h"
#include "tween.h"
#include "utils.h"

typedef int NodeId;

#define BASE_COLOR \
    (Color) { 100, 100, 100, 200 }

#define HIGHLIGHT_COLOR \
    (Color) { 0, 200, 200, 200 }

#define ALT_COLOR \
    (Color) { 100, 255, 255, 200 }

struct NState {
    uint8_t active;
    uint8_t hot;
    uint8_t init;
    int cursor;
    float value;
    float last_value;
    Vector4 v;
};

static struct {
    struct NState states[4096];
    float px, py;
    Font font;

    bool locked;
    bool a_el_is_hot;
} GuiState = {.px = 0.0f, .py = 0.0f, .locked = false};

void InitGui();
void UpdateGui();

void DoPanel(NodeId id, float x, float y, float width, float height);
void DoFrame(NodeId id, float x, float y, float width, float height,
             float alpha);
bool DoBtn(NodeId id, float x, float y, float width, float height,
           const char* text);
bool DoClickRegion(NodeId id, float x, float y, float width, float height);
void DoCenterXLabel(NodeId id, float outer_width, float y, int font_size,
                    const char* text);

void DoLabel(NodeId id, const char* str, float x, float y, float width,
             float height, int font_size);

void DoModal();
bool DoTextInput(NodeId id, char* buffer, size_t buffer_size, float x, float y,
                 float width, float height);

int DoToggleGroupV(NodeId id, const char* names, float x, float y,
                   float* out_width);

bool DoCheckBox(NodeId id, float x, float y, float width, float height);
bool DoCollapsingHeader(NodeId id, const char* label, float x, float y,
                        float width, float height);

Vector4 CTV4(Color c);
Color V4TC(Vector4 v);

Color DoColorPicker(NodeId* id, float x, float y, float width, float height);

float DoSlider(NodeId id, float x, float y, float width, float height,
               float min, float max);

bool DoDragFloat(NodeId id, float x, float y, float width, float height,
                 float* value, float step);

bool DoDragFloat3(NodeId* id, float x, float y, float width, float height,
                  Vector3* value, float step);

bool DoColorDragFloat4(NodeId* id, float x, float y, float width, float height,
                       Color* color);

int DoIncrementer(NodeId id, float x, float y, float width, float height,
                  int* v, float font_size);

bool IsMouseOnUiElement();

Font GetFont();

void Lock();
void Unlock();

bool Hot(NodeId id);
bool Active(NodeId id);

#endif  // BENIS_GUI_H