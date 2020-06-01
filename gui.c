#include "gui.h"

#define CHECK_IF_HOT()                                                  \
    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height}; \
    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);      \
    if (state->hot) GuiState.a_el_is_hot = true;

#define GET_STATE() struct NState* state = &GuiState.states[id];

void InitGui() {
    // GuiState.font = LoadFont("resources/HappyTime.otf");
    GuiState.font = GetFontDefault();
    GuiState.a_el_is_hot = false;
}

void DoPanel(NodeId id, float x, float y, float width, float height) {
    GET_STATE()

    Color color = (!Hot(id)) ? BASE_COLOR : (Color){200, 200, 200, 255};

    CHECK_IF_HOT()

    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3, ALT_COLOR);
}

void DoFrame(NodeId id, float x, float y, float width, float height,
             float alpha) {
    GET_STATE()
    Color color = (Color){100, 100, 100, (unsigned char)(255 * alpha)};

    CHECK_IF_HOT()

    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3,
                         (Color){200, 200, 200, 200});
}

void DoLabel(NodeId id, const char* str, float x, float y, float width,
             float height, int font_size) {
    GET_STATE()
    CHECK_IF_HOT()

    DrawTextRec(GuiState.font, str, (Rectangle){x, y, width, height}, font_size,
                1, true, HIGHLIGHT_COLOR);
}

bool DoClickRegion(NodeId id, float x, float y, float width, float height) {
    GET_STATE()
    CHECK_IF_HOT()
    state->active = state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    return Active(id);
}

bool DoBtn(NodeId id, float x, float y, float width, float height,
           const char* text) {
    GET_STATE()
    CHECK_IF_HOT()

    state->active = state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    const float size = 30.0f;

    Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);

    // Draw the button
    DoPanel(id, x, y, width, height);
    DrawTextEx(GuiState.font, text,
               (Vector2){
                   x + width / 2 - measure.x / 2,
                   y + height / 2 - measure.y / 2,
               },
               size, 1, HIGHLIGHT_COLOR);

    return Active(id);
}

bool DoTexBtn(NodeId id, float x, float y, float width, float height,
              const char* text, Texture2D texture) {
    GET_STATE()
    CHECK_IF_HOT()

    state->active = state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    const float size = 30.0f;

    Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);

    // Draw the button
    DoPanel(id, x, y, width, height);

    Color color = {200, 200, 200, 255};
    if (state->hot) color = WHITE;

    DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height},
                   (Rectangle){x + 1, y + 1, width - 2, height - 2},
                   (Vector2){0.0f, 0.0f}, 0.0f, color);

    DrawTextEx(GuiState.font, text,
               (Vector2){
                   x + width / 2 - measure.x / 2,
                   y + height / 2 - measure.y / 2,
               },
               size, 1, HIGHLIGHT_COLOR);

    return Active(id);
}

void DoCenterXLabel(NodeId id, float outer_width, float y, int font_size,
                    const char* text) {
    Vector2 measure = MeasureTextEx(GuiState.font, text, font_size, 1);

    DrawTextEx(GuiState.font, text,
               (Vector2){
                   outer_width / 2 - measure.x / 2,
                   y,
               },
               font_size, 1, HIGHLIGHT_COLOR);
}

void DoModal() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  (Color){0, 0, 0, 200});
}

bool DoTextInput(NodeId id, char* buffer, size_t buffer_size, float x, float y,
                 float width, float height) {
    GET_STATE()
    DoPanel(id, x, y, width, height);

    const int fsize = 30;
    Vector2 size = MeasureTextEx(GuiState.font, buffer, fsize, 1);
    DrawTextEx(GuiState.font, buffer,
               (Vector2){x + 4, y + height / 2 - size.y / 2}, fsize, 1, BLACK);

    CHECK_IF_HOT()

    if (!state->active) {
        state->active = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && state->hot;
    } else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !state->hot)
            state->active = false;

        if (IsKeyDown(KEY_BACKSPACE)) {
            if (state->cursor > 0) {
                buffer[state->cursor - 1] = '\0';
                state->cursor--;
            }
        }

        int key = GetKeyPressed();
        if (key > 0) {
            buffer[state->cursor++] = (char)key;
        }
    }

    if (state->active && (((int)(GetTime() * 3.0f) % 2) == 0)) {
        DrawRectangle(x + 4 + size.x, y + 4, 4, height - 8, BLACK);
    }

    return state->active && IsKeyPressed(KEY_ENTER);
}

int DoToggleGroupV(NodeId id, const char* names, float x, float y,
                   float* out_width) {
    GET_STATE()

    char* it = names;
    char* end = it + strlen(names);

    float cursor_y = 0;
    const float r = 10;

    int which = 0;

    while (it != end && *it != '\0') {
        char* start = it;

        while (it != end && &it != '\0') {
            if (*it == '|') {
                tstr name = talloc(it - start + 1);
                sprintf(name, "%.*s", (int)(it - start), start);

                Vector2 size = MeasureTextEx(GuiState.font, name, 20, 1);

                state->hot = CheckCollisionPointRec(
                    GetMousePosition(),
                    (Rectangle){x, y + cursor_y, size.x + r * 2 + 6,
                                size.y + 4});

                if (state->hot) GuiState.a_el_is_hot = true;

                DoPanel(id, x, y + cursor_y, size.x + r * 2 + 6, size.y + 4);
                DrawTextEx(GuiState.font, name,
                           (Vector2){x + r * 2 + 2, y + cursor_y + 2}, 20, 1,
                           HIGHLIGHT_COLOR);

                DrawCircle(x + r + 2, y + 2 + cursor_y + size.y / 2, r,
                           BASE_COLOR);
                DrawCircleLines(x + r + 2, y + 2 + cursor_y + size.y / 2, r,
                                ALT_COLOR);

                if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    state->cursor = which;
                }

                if (which == state->cursor) {
                    DrawCircle(x + r + 2.0f, y + 2 + cursor_y + size.y / 2.0f,
                               r * 0.8f, ALT_COLOR);
                }

                state->hot = false;

                cursor_y += size.y + 4;

                which++;

                break;
            }
            it++;
        }

        it++;
    }

    if (out_width != NULL) *out_width = cursor_y;

    return state->cursor;
}

float DoSlider(NodeId id, float x, float y, float width, float height,
               float min, float max) {
    GET_STATE()

    float drag_x = x + width * (state->value / max);

    if (state->init == 0) {
        state->last_value = drag_x;
        state->value = min;
        state->init = 1;
    }

    CHECK_IF_HOT()

    float mx = (GetMouseX() - x);
    drag_x = x + mx;

    state->active =
        state->hot &&
        CheckCollisionPointRec(GetMousePosition(),
                               (Rectangle){drag_x, y, width, width}) &&
        IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (state->active) {
        state->last_value = drag_x;
        float perc = mx / width;
        state->value = min + (max - min) * perc;
    }

    Color color = BASE_COLOR;
    if (state->active) color.a = 255;

    DoPanel(id, x, y, width, height);

    DrawRectangle(state->last_value, y, height, height, color);
    DrawRectangleLines(state->last_value, y, height, height, ALT_COLOR);

    return state->value;
}

Color DoColorPicker(NodeId* id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[*id];

    if (!state->init) {
        state->v = (Vector4){1, 1, 1, 1};
        state->init = 1;
    }

    DoDragFloat((*id)++, x + height, y, (width - height), height / 4,
                &state->v.x, 0.01f);

    DoDragFloat((*id)++, x + height, y + height / 4, (width - height),
                height / 4, &state->v.y, 0.01f);

    DoDragFloat((*id)++, x + height, y + (height / 4) * 2, (width - height),
                height / 4, &state->v.z, 0.01f);

    DoDragFloat((*id)++, x + height, y + (height / 4) * 3, (width - height),
                height / 4, &state->v.w, 0.01f);

    state->v.x = max(min(state->v.x, 1.0f), 0.0f);
    state->v.y = max(min(state->v.y, 1.0f), 0.0f);
    state->v.z = max(min(state->v.z, 1.0f), 0.0f);
    state->v.w = max(min(state->v.w, 1.0f), 0.0f);

    Color color = V4TC(state->v);
    DrawRectangle((int)x, (int)y, (int)height, (int)height, color);
    return color;
}

bool DoCheckBox(NodeId id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[id];

    CHECK_IF_HOT()

    if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state->active = !state->active;
    }

    // Draw the button
    DoPanel(id, x, y, width, height);

    if (state->active)
        DrawRectangle((int)x + 1, (int)y + 1, (int)width - 2, (int)height - 2,
                      ALT_COLOR);

    return Active(id);
}

bool DoCollapsingHeader(NodeId id, const char* label, float x, float y,
                        float width, float height) {
    struct NState* state = &GuiState.states[id];

    CHECK_IF_HOT()

    if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state->active = !state->active;
    }

    char buff[512];
    sprintf(buff, "%s %s", (state->active ? "-" : "+"), label);

    const Vector2 size = MeasureTextEx(GuiState.font, buff, 30, 1.0f);

    // Draw the button
    DoPanel(id, x, y, width, height);
    DoLabel(id, buff, x + width / 2 - size.x / 2, y + 1, width, height, 30);

    return Active(id);
}

bool DoDragRegionFloat(NodeId id, float x, float y, float width, float height,
                       float* value, float step) {
    struct NState* state = &GuiState.states[id];

    CHECK_IF_HOT()

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

bool DoDragFloat(NodeId id, float x, float y, float width, float height,
                 float* value, float step) {
    struct NState* state = &GuiState.states[id];

    DoDragRegionFloat(id, x, y, width, height, value, step);

    const char* str = TextFormat("%.2f", *value);

    Vector2 size = MeasureTextEx(GuiState.font, str, 20, 1);

    DoPanel(id, x, y, width, height);
    DrawTextRec(GuiState.font, str,
                (Rectangle){x + width / 2 - size.x / 2,
                            y + height / 2 - size.y / 2, width, height},
                20, 1, true, HIGHLIGHT_COLOR);

    return Active(id);
}

bool DoDragFloat3(NodeId* id, float x, float y, float width, float height,
                  Vector3* value, float step) {
    int nid = *id;
    DoDragFloat(nid++, x + width / 3 * 0, y, width / 3, height, &value->x,
                step);
    DoDragFloat(nid++, x + width / 3 * 1, y, width / 3, height, &value->y,
                step);
    DoDragFloat(nid++, x + width / 3 * 2, y, width / 3, height, &value->z,
                step);
    *id = nid;
}

bool DoColorDragFloat4(NodeId* id, float x, float y, float width, float height,
                       Color* color) {
    struct NState* state = &GuiState.states[(*id)];

    if (state->init == 0) {
        state->init = 1;

        state->v = (Vector4){color->r / 255.0f, color->g / 255.0f,
                             color->b / 255.0f, color->a / 255.0f};
    }

    DrawRectangle(x, y, height, height, *color);

    float delta = 0;
    DoDragRegionFloat(*id, x, y, height, height, &delta, (1.0f / 255.0f));

    if (delta > 0 || delta < 0) {
        state->v.x += delta;
        state->v.y += delta;
        state->v.z += delta;
    }

    (*id)++;

    DoDragFloat3(id, x + height, y, width - height, height, &state->v,
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

int DoIncrementer(NodeId id, float x, float y, float width, float height,
                  int* v, float font_size) {
    struct NState* state = &GuiState.states[id];

    CHECK_IF_HOT()

    state->active = state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    const float size = font_size;

    const char* text = FormatText("%d", *v);

    Vector2 measure = MeasureTextEx(GuiState.font, text, size, 1);
    DoPanel(id, x, y, width, height);
    DrawTextEx(GuiState.font, text,
               (Vector2){
                   x + width / 2 - measure.x / 2,
                   y + height / 2 - measure.y / 2,
               },
               size, 1, HIGHLIGHT_COLOR);

    if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *v = (*v) + 1;
        return 1;
    } else if (state->hot && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        *v = (*v) - 1;
        return -1;
    }

    return 0;
}

void BeginScrollPanelV(NodeId id, float x, float y, float width, float height,
                       float* scroll, float max_height) {
    const float scroll_w = 12;

    GET_STATE()
    CHECK_IF_HOT()

    if (state->hot && scroll != NULL) {
        int mdelta = GetMouseWheelMove();
        (*scroll) -= mdelta * 20.0f;
    }

    if (*scroll < 0) {
        *scroll = 0.0f;
    }

    if (*scroll > max_height) *scroll = max_height;

    float ratio = (height / max_height);
    float scroll_ratio =
        ((*scroll < 0.0f ? -(*scroll) : (*scroll)) / max_height);

    DoFrame(id, x, y, width, height, 0.8f);

    DrawRectangleLines(x + width - scroll_w, y, scroll_w, height,
                       HIGHLIGHT_COLOR);

    // TODO(Dustin): Allow dragging the scrollbar
    DrawRectangle(x + width - scroll_w, y + height * scroll_ratio, scroll_w,
                  height * ratio, HIGHLIGHT_COLOR);

    BeginScissorMode(x, y, width - scroll_w, height);
}

void EndScrollPanelV() { EndScissorMode(); }

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

void UpdateGui() { GuiState.a_el_is_hot = false; }

bool IsMouseOnUiElement() { return GuiState.a_el_is_hot; }

Font GetFont() { return GuiState.font; }

void Lock() { GuiState.locked = true; }

void Unlock() { GuiState.locked = false; }

bool Active(NodeId id) {
    return GuiState.states[id].active && !GuiState.locked;
}

bool Hot(NodeId id) { return GuiState.states[id].hot && !GuiState.locked; }