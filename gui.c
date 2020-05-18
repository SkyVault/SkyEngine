#include "gui.h"

void InitGui() { GuiState.font = LoadFont("resources/alagard_font.png"); }

void DoPanel(NodeId id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[id];

    Color color = (!Hot(id)) ? BASE_COLOR : (Color){200, 200, 200, 255};

    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3, ALT_COLOR);
}

void DoFrame(NodeId id, float x, float y, float width, float height,
             float alpha) {
    struct NState* state = &GuiState.states[id];
    Color color = (Color){100, 100, 100, (unsigned char)(255 * alpha)};
    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3,
                         (Color){200, 200, 200, 200});
}

void DoLabel(NodeId id, const char* str, float x, float y, float width,
             float height, int font_size) {
    struct NState* state = &GuiState.states[id];

    DrawTextRec(GuiState.font, str, (Rectangle){x, y, width, height}, font_size,
                1, true, HIGHLIGHT_COLOR);
}

bool DoClickRegion(NodeId id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[id];
    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};
    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);
    state->active = state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    return Active(id);
}

bool DoBtn(NodeId id, float x, float y, float width, float height,
           const char* text) {
    struct NState* state = &GuiState.states[id];

    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};

    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);
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
    struct NState* state = &GuiState.states[id];
    DoPanel(id, x, y, width, height);

    const int fsize = 25;
    Vector2 size = MeasureTextEx(GuiState.font, buffer, fsize, 1);
    DrawTextEx(GuiState.font, buffer,
               (Vector2){x + 4, y + height / 2 - size.y / 2}, fsize, 1, BLACK);

    state->hot = CheckCollisionPointRec(GetMousePosition(),
                                        (Rectangle){x, y, width, height});

    if (!state->active) {
        state->active = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && state->hot;
    } else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !state->hot)
            state->active = false;

        if (IsKeyPressed(KEY_BACKSPACE)) {
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
    struct NState* state = &GuiState.states[id];

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

                DoPanel(id, x, y + cursor_y, size.x + r * 2 + 6, size.y + 4);
                DrawTextEx(GuiState.font, name,
                           (Vector2){x + r * 2 + 2, y + cursor_y + 2}, 20, 1,
                           HIGHLIGHT_COLOR);

                DrawCircle(x + r + 2, y + 2 + cursor_y + size.y / 2, r,
                           HIGHLIGHT_COLOR);
                DrawCircleLines(x + r + 2, y + 2 + cursor_y + size.y / 2, r,
                                BLACK);

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
    struct NState* state = &GuiState.states[id];

    float drag_x = x + width * (state->value / max);

    if (state->init == 0) {
        state->last_value = drag_x;
        state->value = min;
        state->init = 1;
    }

    state->hot = CheckCollisionPointRec(GetMousePosition(),
                                        (Rectangle){x, y, width, height});

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

Color DoColorPicker(NodeId id, float x, float y, float width, float height,
                    Color color) {
    DrawRectangle(x, y, height, height, color);
}

bool DoCheckBox(NodeId id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[id];

    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};

    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);

    if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        state->active = !state->active;
    }

    // Draw the button
    DoPanel(id, x, y, width, height);

    if (state->active)
        DrawRectangle(x + 1, y + 1, width - 2, height - 2, ALT_COLOR);

    return Active(id);
}

bool DoCollapsingHeader(NodeId id, const char* label, float x, float y,
                        float width, float height) {
    struct NState* state = &GuiState.states[id];

    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};

    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);

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

bool DoDragFloat(NodeId id, float x, float y, float width, float height,
                 float* value, float step) {
    struct NState* state = &GuiState.states[id];

    Rectangle rect = {x + GuiState.px, y + GuiState.py, width, height};
    state->hot = CheckCollisionPointRec(GetMousePosition(), rect);

    bool last = state->active;
    state->active = state->hot && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    if (!last && state->active) {
        state->last_value = GetMouseX();
    }

    if (last && state->active) {
        *value += (state->last_value - GetMouseX()) * step;
        state->last_value = GetMouseX();
    }

    const char* str = TextFormat("%f", *value);

    Vector2 size = MeasureTextEx(GuiState.font, str, 20, 1);

    DoPanel(id, x, y, width, height);
    DrawTextRec(GuiState.font, str,
                (Rectangle){x + width / 2 - size.x / 2, y, width, height}, 20,
                1, true, HIGHLIGHT_COLOR);

    return Active(id);
}

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

void Lock() { GuiState.locked = true; }

void Unlock() { GuiState.locked = false; }

bool Active(NodeId id) {
    return GuiState.states[id].active && !GuiState.locked;
}

bool Hot(NodeId id) { return GuiState.states[id].hot && !GuiState.locked; }