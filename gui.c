#include "gui.h"

void InitGui() { GuiState.font = LoadFont("resources/alagard_font.png"); }

void DoPanel(NodeId id, float x, float y, float width, float height) {
    struct NState* state = &GuiState.states[id];

    Color color =
        (!Hot(id)) ? (Color){100, 100, 100, 255} : (Color){200, 200, 200, 255};

    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3, WHITE);
}

void DoFrame(NodeId id, float x, float y, float width, float height,
             float alpha) {
    struct NState* state = &GuiState.states[id];
    Color color = (Color){100, 100, 100, (unsigned char)(255 * alpha)};
    DrawRectangle(x, y, width, height, color);
    DrawRectangleLinesEx((Rectangle){x, y, width, height}, 3,
                         (Color){200, 200, 200, 200});
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
               size, 1, WHITE);

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
               font_size, 1, WHITE);
}

void DoModal() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  (Color){0, 0, 0, 200});
}

void DoTextInput(NodeId id, char* buffer, size_t buffer_size, float x, float y,
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
                           BLACK);

                DrawCircle(x + r + 2, y + 2 + cursor_y + size.y / 2, r, WHITE);
                DrawCircleLines(x + r + 2, y + 2 + cursor_y + size.y / 2, r,
                                BLACK);

                if (state->hot && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    state->cursor = which;
                }

                if (which == state->cursor) {
                    DrawCircle(x + r + 2.0f, y + 2 + cursor_y + size.y / 2.0f,
                               r * 0.8f, RED);
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

void Lock() { GuiState.locked = true; }

void Unlock() { GuiState.locked = false; }

bool Active(NodeId id) {
    return GuiState.states[id].active && !GuiState.locked;
}

bool Hot(NodeId id) { return GuiState.states[id].hot && !GuiState.locked; }