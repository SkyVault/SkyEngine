#ifndef BENIS_GUI_H
#define BENIS_GUI_H

#include <stdbool.h>

#include "raylib.h"
#include "tween.h"

typedef int NodeId;

struct NState {
    int active;
    int hot;
    float value;
};

static struct {
    struct NState states[1000];
    float px, py;
    Font font;

    bool locked;
} GuiState = {.px = 0.0f, .py = 0.0f, .locked = false};

void InitGui();
void DoFrame(NodeId id, float x, float y, float width, float height);
bool DoBtn(NodeId id, float x, float y, float width, float height,
           const char* text);
void DoCenterXLabel(NodeId id, float outer_width, float y, int font_size,
                    const char* text);

void DoModal();

void Lock();
void Unlock();

bool Hot(NodeId id);
bool Active(NodeId id);

#endif  // BENIS_GUI_H