#include "main_menu.h"

void spread_fire(MainMenuState *self, int src) {
  int rnd = (int)floor((rand_f() * 3.0f)) & 3;
  int dst = src - rnd + 1;

  int dst_f = dst - FIRE_WIDTH;
  int src_f = src;

  if (dst_f >= 0 && dst_f < FIRE_WIDTH*FIRE_HEIGHT &&
      src_f >= 0 && src_f < FIRE_WIDTH*FIRE_HEIGHT) {
    self->fire_pixels[dst - FIRE_WIDTH] = self->fire_pixels[src] - (rnd & 1);
  }
}

void do_fire(MainMenuState *self) {
  for (int x = 0; x < FIRE_WIDTH; x++) {
    for (int y = FIRE_HEIGHT - 1; y >= 1; y--) {
      spread_fire(self, y * FIRE_WIDTH + x);
    }
  }
}

void main_menu_function(void* data, int state) {
    MainMenuState *self = (MainMenuState*)data;

    switch (state) {
        case SCENE_STATE_LOAD: {
            printf("MAIN MENU LOADED\n");

            self->do_controls_modal = false;
            self->do_exit_modal = false;

            self->target = LoadRenderTexture(FIRE_WIDTH, FIRE_HEIGHT);

            for (size_t i = 0; i < (NUM_RGBS / 3); i++)
                self->palette[i] = (Color) {
                    rgbs[i * 3 + 0],
                    rgbs[i * 3 + 1],
                    rgbs[i * 3 + 2],
                    255,
                };

            for (int i = 0; i < FIRE_WIDTH * FIRE_HEIGHT; i++)
                self->fire_pixels[i] = 0;

            for (int i = 0; i < FIRE_WIDTH; i++)
                self->fire_pixels[i + (FIRE_HEIGHT - 1) * FIRE_WIDTH] = 36;

            break;
        }
        case SCENE_STATE_UPDATE: {
            do_fire(self);
            update_gui();
            break;
        }
        case SCENE_STATE_RENDER: {
            EnableCursor();

            Game *game = self->game;
            Assets *assets = self->game->assets;
            Camera *camera = self->game->camera;
            Shader *shader = &assets->shaders[SHADER_PHONG_LIGHTING];

            BeginDrawing();
            ClearBackground((Color){100, 155, 155, 255});

            const float height = FIRE_HEIGHT * 4;

            // Draw fun fire thingy
            BeginTextureMode(self->target);
            ClearBackground((Color){0, 0, 0, 0});
            for (int x = 0; x < FIRE_WIDTH; x++) {
                for (int y = 0; y < FIRE_HEIGHT; y++) {
                    int i = self->fire_pixels[y * FIRE_WIDTH + x];

                    if (!self->do_exit_modal && GetTime() < 20.0f) {
                        float c = (float)i / 36.0f;
                        if (c > 1.0f) c = 1.0f;
                        if (c < 0.0f) c = 0.0f;
                        Vector4 v = (Vector4){1, 1, 1, c};
                        Color color = VEC4_TO_COLOR(v);
                        DrawRectangle(x, FIRE_HEIGHT - y, 1, 1, color);
                    } else {
                        DrawRectangle(x, FIRE_HEIGHT - y, 1, 1, self->palette[i]);
                    }
                }
            }
            EndTextureMode();

            const int h =
                GetScreenWidth() * (((float)(FIRE_HEIGHT)) / ((float)(FIRE_WIDTH)));

            DrawTexturePro(self->target.texture,
                            (Rectangle){0, 0, self->target.texture.width, self->target.texture.height},
                            (Rectangle){0, GetScreenHeight() - h, GetScreenWidth(), h},
                            Vector2Zero(), 0.0f, WHITE);

            // Buttons
            float x = GetScreenWidth() - 20;
            float y = GetScreenHeight() - 20;

            const float shw = GetScreenWidth() / 2;
            const float shh = GetScreenHeight() / 2;

            do_center_x_label(GetScreenWidth(), 30, 100, VAULT_TITLE);

            do_center_x_label(GetScreenWidth() + cosf(GetTime() * 2) * 50.0f, 120, 30,
                                TextFormat("Edit Version: %s", VAULT_VERSION));

            // Buttons
            int id = 0;
            const float btn_w = GetScreenWidth() / 2.2f;
            const float btn_h = 50.0f;
            const float margin = 10.0f;
            const float tot_h = (btn_h + margin) * 5;
            const float oy = 0.0f;

            static float timer = 0.0f;
            timer += GetFrameTime() * 0.3f;
            if (timer > 1.0f)
                timer = 1.0f;

            static float scaler_t = 0.0f;

            const int EASING = TWEEN_BOUNCEOUT;

            #define GEASE(scale)                                                           \
                float t = timer * scale;                                                     \
                if (t > 1.0f)                                                                \
                    t = 1.0f;                                                                  \
                float mo = Hot(id) ? (float)ease(TWEEN_LINEAR, scaler_t) * 30.0f : 0.0f;     \
                const float xx = -btn_w + (float)ease(EASING, t) * btn_w

            #define DO_BTN(txt)                                                            \
                do_btn(shw + xx - btn_w / 2 - mo / 2,                                        \
                        (shh - (tot_h / 2)) + (btn_h + margin) * id - mo / 2, btn_w + mo,     \
                        btn_h + mo, txt)

            #define DO_SCALING()                                                           \
                if (Hot(id))                                                                 \
                scaler_t += GetFrameTime() * 2.0f

            bool shrink = true;
            #if 1
            {
                DO_SCALING();
                GEASE(1.0f);
                if (DO_BTN("Start New Game :)")) {
                    game->scene = SCENE_GAME;
                    reset_gui();
                }
                if (Hot(id)) shrink = false;
                ++id;
            }

            {
                DO_SCALING(); GEASE(1.1f);
                if (DO_BTN("Load Game")) {}
                if (Hot(id)) shrink = false;
                ++id;
            }

            {
                DO_SCALING(); GEASE(1.2f);
                if (DO_BTN("Settings")) {}
                if (Hot(id)) shrink = false;
                ++id;
            }

            int first = false;
            {
                DO_SCALING(); GEASE(1.2f);
                if (DO_BTN("Controls") && !self->do_controls_modal) {
                    first = true;
                    self->do_controls_modal = true;
                }
                if (Hot(id)) shrink = false;
                ++id;
            }

            {
                DO_SCALING(); GEASE(1.3f);
                if (DO_BTN("Exit :("))
                    self->do_exit_modal = true;
                if (Hot(id))
                    shrink = false;
                ++id;
            }
            #endif

            if (do_btn(shw + btn_w / 2 + 20, (shh - (tot_h / 2)), 200, btn_h, "Edit")) {
                game->scene = SCENE_EDIT;
            }

            if (shrink)
                scaler_t -= GetFrameTime();
            if (scaler_t > 1.0f)
                scaler_t = 1.0f;
            if (scaler_t < 0.0f)
                scaler_t = 0.0f;

            if (self->do_controls_modal) {
                do_modal();
                Unlock();

                do_center_x_label(GetScreenWidth(), 130, 30, "W|A|S|D / move");
                do_center_x_label(GetScreenWidth(), 160, 30, "Left click / throw orange");
                do_center_x_label(GetScreenWidth(), 190, 30,
                                "Right click / throw pineapple bomb");
                do_center_x_label(GetScreenWidth(), 220, 30, "Escape / release mouse");

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !first) {
                    self->do_controls_modal = false;
                }

                if (first) first = false;

                Lock();
            } else if (self->do_exit_modal) {
                do_modal();

                Unlock();
                const float x = shw - 50;
                const float y = shh - 100;
                do_center_x_label((float)GetScreenWidth(), 200, 60, "Are you sure?");
                if (do_btn(x - 55, y, 100, 100, "YES")) {
                    self->do_exit_modal = false;
                    game->state = STATE_QUITTING;
                }
                if (do_btn(x + 55, y, 100, 100, "NO"))
                    self->do_exit_modal = false;
                Lock();
            } else {
                Unlock();
            }


            EndDrawing();
            break;
        }
        case SCENE_STATE_DESTROY: {
            break;
        }
    }
}
