#include <GL/glew.h>
#include <janet.h>
#include <stdio.h>

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT ((int)((SCREEN_WIDTH) * (9.0f / 16.0f)))

#include "assembler.h"
#include "assets.h"
#include "behaviours.h"
#include "constants.h"
#include "dict.h"
#include "ecs.h"
#include "editor.h"
#include "game.h"
#include "gameworld.h"
#include "gui.h"
#include "particles.h"
#include "raylib.h"
#include "raymath.h"
#include "renderer.h"
#include "rlgl.h"
#include "rlights.h"
#include "tween.h"

// #define PLATFORM_DESKTOP

#define BUFFSIZE (512)

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else
#define GLSL_VERSION 100
#endif

#define BENIS_VERSION_MAJOR "0"
#define BENIS_VERSION_MINOR "1"
#define BENIS_VERSION_PATCH "15"
#define BENIS_VERSION \
    BENIS_VERSION_MAJOR "." BENIS_VERSION_MINOR "." BENIS_VERSION_PATCH

#define MAX_COLUMNS (20)
#define MAIN_SHADER SHADER_PHONG_LIGHTING

void custom_logger(int msg_type, const char *text, va_list args) {
    // printf(text, args);
    // printf("\n");
}

float timer = 0.0f;

#define FIRE_WIDTH (1280 / 4)
#define FIRE_HEIGHT (96)

const int rgbs[] = {
    0x07, 0x07, 0x07, 0x1F, 0x07, 0x07, 0x2F, 0x0F, 0x07, 0x47, 0x0F, 0x07,
    0x57, 0x17, 0x07, 0x67, 0x1F, 0x07, 0x77, 0x1F, 0x07, 0x8F, 0x27, 0x07,
    0x9F, 0x2F, 0x07, 0xAF, 0x3F, 0x07, 0xBF, 0x47, 0x07, 0xC7, 0x47, 0x07,
    0xDF, 0x4F, 0x07, 0xDF, 0x57, 0x07, 0xDF, 0x57, 0x07, 0xD7, 0x5F, 0x07,
    0xD7, 0x5F, 0x07, 0xD7, 0x67, 0x0F, 0xCF, 0x6F, 0x0F, 0xCF, 0x77, 0x0F,
    0xCF, 0x7F, 0x0F, 0xCF, 0x87, 0x17, 0xC7, 0x87, 0x17, 0xC7, 0x8F, 0x17,
    0xC7, 0x97, 0x1F, 0xBF, 0x9F, 0x1F, 0xBF, 0x9F, 0x1F, 0xBF, 0xA7, 0x27,
    0xBF, 0xA7, 0x27, 0xBF, 0xAF, 0x2F, 0xB7, 0xAF, 0x2F, 0xB7, 0xB7, 0x2F,
    0xB7, 0xB7, 0x37, 0xCF, 0xCF, 0x6F, 0xDF, 0xDF, 0x9F, 0xEF, 0xEF, 0xC7,
    0xFF, 0xFF, 0xFF};

#define NUM_RGBS (sizeof(rgbs) / sizeof(rgbs[0]))
typedef struct {
    int fire_pixels[FIRE_WIDTH * FIRE_HEIGHT];
    Color pallet[NUM_RGBS / 3];
    RenderTexture2D target;
} MainMenuState;

void spread_fire(MainMenuState *state, int src) {
    int rnd = (int)floor((rand_f() * 3.0f)) & 3;
    int dst = src - rnd + 1;
    state->fire_pixels[dst - FIRE_WIDTH] = state->fire_pixels[src] - (rnd & 1);
}

void do_fire(MainMenuState *state) {
    for (int x = 0; x < FIRE_WIDTH; x++) {
        for (int y = FIRE_HEIGHT - 1; y >= 1; y--) {
            spread_fire(state, y * FIRE_WIDTH + x);
        }
    }
}

bool do_controls_modal = false;
bool do_exit_modal = false;

void update_and_render_menu_scene(MainMenuState *state, Game *game,
                                  EcsWorld *ecs, ParticleSystem *particle_sys,
                                  Region *self, GfxState *gfx) {
    Assets *assets = game->assets;
    Camera *camera = game->camera;

    Shader *shader = &assets->shaders[MAIN_SHADER];

    timer += GetFrameTime() * 0.2f;
    if (timer > 1.0f) {
        timer = 1.0f;
    }

    // TWEEN_SCHUBRING1,  // terry schubring's formula 1
    float rot =
        360 - ((ease(TWEEN_BOUNCEOUT, timer) * 180.0f) - 180) - (45 / 2);

    if (IsKeyPressed(KEY_ENTER)) {
        game->scene = SCENE_GAME;
    }

    // symulate the fire
    do_fire(state);

    BeginDrawing();
    ClearBackground((Color){255, 155, 155, 255});

    const float height = FIRE_HEIGHT * 4;

    // Draw fun fire thingy

    BeginTextureMode(state->target);
    ClearBackground((Color){0, 0, 0, 0});
    for (int x = 0; x < FIRE_WIDTH; x++) {
        for (int y = 0; y < FIRE_HEIGHT; y++) {
            int i = state->fire_pixels[y * FIRE_WIDTH + x];

            if (!do_exit_modal && GetTime() < 20.0f) {
                float c = (float)i / 36.0f;
                if (c > 1.0f) c = 1.0f;
                if (c < 0.0f) c = 0.0f;
                Vector4 v = (Vector4){1, 1, 1, c};
                Color color = VEC4_TO_COLOR(v);
                DrawRectangle(x, FIRE_HEIGHT - y, 1, 1, color);
            } else {
                DrawRectangle(x, FIRE_HEIGHT - y, 1, 1, state->pallet[i]);
            }
        }
    }
    EndTextureMode();

    const int h =
        GetScreenWidth() * (((float)(FIRE_HEIGHT)) / ((float)(FIRE_WIDTH)));

    // DrawTexture(state->target.texture, 0, GetScreenHeight() - (height),
    // WHITE);
    DrawTexturePro(state->target.texture,
                   (Rectangle){0, 0, state->target.texture.width,
                               state->target.texture.height},
                   (Rectangle){0, GetScreenHeight() - h, GetScreenWidth(), h},
                   Vector2Zero(), 0.0f, WHITE);

    Texture2D t = assets->textures[TEX_GIRL_1];

    float x = GetScreenWidth() - 20;
    float y = GetScreenHeight() - 20;

    // DrawTexturePro(t, (Rectangle){0, 0, t.width, t.height},
    //               (Rectangle){x, y, 500, 500}, (Vector2){250, 500}, rot,
    //               WHITE);

    const float shw = GetScreenWidth() / 2;
    const float shh = GetScreenHeight() / 2;

    DoCenterXLabel(100, GetScreenWidth(), 30, 100, "Benis Shooter 3D");

    DoCenterXLabel(100, GetScreenWidth() + cosf(GetTime() * 2) * 50.0f, 120, 30,
                   TextFormat("Edit Version: %s", BENIS_VERSION));

    // Buttons
    int id = 0;
    const float btn_w = GetScreenWidth() / 2.2f;
    const float btn_h = 50.0f;
    const float margin = 10.0f;
    const float tot_h = (btn_h + margin) * 5;
    const float oy = 0.0f;

    static float timer = 0.0f;
    timer += GetFrameTime() * 0.3f;
    if (timer > 1.0f) timer = 1.0f;

    static float scaler_t = 0.0f;

    const int EASING = TWEEN_BOUNCEOUT;

#define GEASE(scale)                                                         \
    float t = timer * scale;                                                 \
    if (t > 1.0f) t = 1.0f;                                                  \
    float mo = Hot(id) ? (float)ease(TWEEN_LINEAR, scaler_t) * 30.0f : 0.0f; \
    const float xx = -btn_w + (float)ease(EASING, t) * btn_w

#define DO_BTN(txt)                                                         \
    DoBtn(id, shw + xx - btn_w / 2 - mo / 2,                                \
          (shh - (tot_h / 2)) + (btn_h + margin) * id - mo / 2, btn_w + mo, \
          btn_h + mo, txt)

#define DO_SCALING() \
    if (Hot(id)) scaler_t += GetFrameTime() * 2.0f

    bool shrink = true;
#if 1
    {
        DO_SCALING();
        GEASE(1.0f);
        if (DO_BTN("Start New Game :)")) {
            game->scene = SCENE_GAME;
            ResetGui();
        }
        if (Hot(id)) shrink = false;
        ++id;
    }

    {
        DO_SCALING();
        GEASE(1.1f);
        if (DO_BTN("Load Game")) {
        }
        if (Hot(id)) shrink = false;
        ++id;
    }

    {
        DO_SCALING();
        GEASE(1.2f);
        if (DO_BTN("Settings")) {
        }
        if (Hot(id)) shrink = false;
        ++id;
    }

    int first = false;
    {
        DO_SCALING();
        GEASE(1.2f);
        if (DO_BTN("Controls") && !do_controls_modal) {
            first = true;
            do_controls_modal = true;
        }
        if (Hot(id)) shrink = false;
        ++id;
    }

    {
        DO_SCALING();
        GEASE(1.3f);
        if (DO_BTN("Exit :(")) do_exit_modal = true;
        if (Hot(id)) shrink = false;
        ++id;
    }
#endif

    if (DoBtn(id++, shw + 300, (shh - (tot_h / 2)), 200, btn_h, "Edit")) {
        game->scene = SCENE_EDIT;
    }

    if (shrink) scaler_t -= GetFrameTime();
    if (scaler_t > 1.0f) scaler_t = 1.0f;
    if (scaler_t < 0.0f) scaler_t = 0.0f;

    if (do_controls_modal) {
        DoModal();
        Unlock();

        DoCenterXLabel(++id, GetScreenWidth(), 130, 30, "W|A|S|D / move");
        DoCenterXLabel(++id, GetScreenWidth(), 160, 30,
                       "Left click / throw orange");
        DoCenterXLabel(++id, GetScreenWidth(), 190, 30,
                       "Right click / throw pineapple bomb");
        DoCenterXLabel(++id, GetScreenWidth(), 220, 30,
                       "Escape / release mouse");

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !first) {
            do_controls_modal = false;
        }

        if (first) first = false;

        Lock();
    } else if (do_exit_modal) {
        DoModal();

        Unlock();
        const float x = shw - 50;
        const float y = shh - 100;
        DoCenterXLabel(++id, (float)GetScreenWidth(), 200, 60, "Are you sure?");
        if (DoBtn(++id, x - 55, y, 100, 100, "YES")) {
            do_exit_modal = false;
            game->state = STATE_QUITTING;
        }
        if (DoBtn(id + 1, x + 55, y, 100, 100, "NO")) do_exit_modal = false;
        Lock();
    } else {
        Unlock();
    }

    EndDrawing();

#undef GEASE
#undef DO_BTN
#undef DO_SCALING
}

void update_and_render_game_scene_with_editor(Game *game, EcsWorld *ecs,
                                              ParticleSystem *particle_sys,
                                              Region *map, GfxState *gfx,
                                              Ed *editor) {
    Assets *assets = game->assets;
    Camera *camera = game->camera;

    Shader *shader = &assets->shaders[MAIN_SHADER];

    update_and_cleanup_ecs_world(ecs);
    update_game(game);
    update_particle_system(particle_sys);

    for (int i = 0; i < ecs->max_num_entities; i++) {
        if (!is_ent_alive(ecs, i)) continue;

        update_billboard(ecs, i);
        update_player(ecs, assets, game, i);
        update_doors(ecs, i);
        update_behaviours(game, ecs, i);
        update_timed_destroy(ecs, i);
        update_physics(map, ecs, game, i);
    }

    update_region(map, game);

    // #if defined _DEBUG
    if (editor != NULL) {
        update_editor(editor, map, game);
    }
    // #endif

    SetShaderValue(*shader, shader->locs[LOC_VECTOR_VIEW], &camera->position,
                   UNIFORM_VEC3);

    Matrix view = GetCameraMatrix(*camera);
    SetShaderValueMatrix(*shader, shader->locs[LOC_MATRIX_VIEW], view);

    BeginDrawing();
    ClearBackground((Color){100, 210, 255, 255});

    // if (IsKeyDown(KEY_TAB)) {
    //     rlEnableWireMode();
    // } else {
    //     rlDisableWireMode();
    // }

    begin_rendering(gfx);
    {
        BeginMode3D(*camera);

        rlViewport(0, 0, gfx->render_texture.texture.width,
                   gfx->render_texture.texture.height);

        render_region(map, gfx, game);

        for (int i = 0; i < ecs->max_num_entities; i++) {
            if (!is_ent_alive(ecs, i)) continue;
            draw_billboard_ent(gfx, camera, ecs, i);
            draw_models(gfx, ecs, i);
        }

        render_particle_system(particle_sys);

        glDisable(GL_CULL_FACE);
        DrawModel(game->skybox, (Vector3){0, 0, 0}, 100.0f, WHITE);
        glEnable(GL_CULL_FACE);

        // Do the final draw to the screen
        flush_graphics(gfx, camera);

        // #if defined _DEBUG
        if (editor != NULL) {
            render_editor(editor, gfx, map, game);
        }
        // #endif

        EndMode3D();
    }
    end_rendering(gfx);

    draw_final_texture_to_screen(gfx);

    // DrawTexture(gfx->render_texture.texture, 0, 0, WHITE);
    UpdateGui();
    draw_player_gui(game, map);

    // #if defined _DEBUG
    if (editor != NULL) {
        render_editor_ui(editor, gfx, map, game);
    }
    // #endif

    DrawFPS(10, 10);
    EndDrawing();
}

void update_and_render_game_scene(Game *game, EcsWorld *ecs,
                                  ParticleSystem *particle_sys, Region *map,
                                  GfxState *gfx) {
    update_and_render_game_scene_with_editor(game, ecs, particle_sys, map, gfx,
                                             NULL);
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    // SetTraceLogLevel(0);
    // SetTraceLogCallback(custom_logger);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DevWindow");
    SetTargetFPS(86);
    SetExitKey(0);

#if 1
    // glew
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    // Init scripting language
    janet_init();

    JanetTable *env = janet_core_env(NULL);
    janet_dostring(env, "(print `Initializing Janet`)", "main", NULL);

    // Define the camera to look into our 3d world (position, target, up
    // vector)
    Camera camera = {0};
    camera.position = (Vector3){1.0f, ACTOR_HEIGHT, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = FOV;
    camera.type = CAMERA_PERSPECTIVE;

    Assets *assets = create_and_load_assets(env);
    EcsWorld *ecs = create_ecs_world();

    Game *game = create_game(assets, &camera, ecs, env);
    GfxState *gfx = create_gfx_state();

    register_janet_c_functions(game);
    assets_load_scripts(assets, env);

    // Load the skybox
    game->skybox = LoadModelFromMesh(assets->meshes[MESH_SKYBOX]);
    game->skybox.materials[0].shader = assets->shaders[SHADER_SKYBOX];
    SetShaderValue(
        game->skybox.materials[0].shader,
        GetShaderLocation(game->skybox.materials[0].shader, "environment_map"),
        (int[1]){MAP_CUBEMAP}, UNIFORM_INT);

    Shader *cubemap_shader = &assets->shaders[SHADER_CUBEMAP];
    SetShaderValue(*cubemap_shader,
                   GetShaderLocation(*cubemap_shader, "equirectagular_map"),
                   (int[1]){0}, UNIFORM_INT);

    Texture2D skybox_texture = LoadTexture("resources/textures/skybox_2.png");
    game->skybox.materials[0].maps[MAP_CUBEMAP].texture =
        GenTextureCubemap(*cubemap_shader, skybox_texture, 512);

    ParticleSystem *particle_sys = create_particle_system();

    InitGui();

    game->map = create_region_from_script("resources/maps/edit.janet", game);
    Region *map = game->map;

    assemble(ACTOR_PLAYER, game, map->player_x, (ACTOR_HEIGHT - GLOBAL_SCALE),
             map->player_z, 0, 0);

    update_game(game);
    EnableCursor();

    // #if defined _DEBUG
    Ed *editor = create_editor();
    // #endif

    rlEnableBackfaceCulling();
    rlEnableDepthTest();
    glClearDepth(1.0f);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    MainMenuState *menu_state = malloc(sizeof(MainMenuState));

    menu_state->target = LoadRenderTexture(FIRE_WIDTH, FIRE_HEIGHT);

    for (int i = 0; i < (NUM_RGBS / 3); i++) {
        menu_state->pallet[i] = (Color){
            rgbs[i * 3 + 0],
            rgbs[i * 3 + 1],
            rgbs[i * 3 + 2],
            255,
        };
    }

    for (int i = 0; i < FIRE_WIDTH * FIRE_HEIGHT; i++)
        menu_state->fire_pixels[i] = 0;

    for (int i = 0; i < FIRE_WIDTH; i++) {
        menu_state->fire_pixels[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] = 36;
    }
#endif

#if 1
    while (!WindowShouldClose() && game->state == STATE_RUNNING) {
        map = game->map;
        update_assets(assets);

        if (assets->shaders_hotload[SHADER_PHONG_LIGHTING].just_hotloaded) {
            // Rebind uniforms
            SetShaderValue(
                assets->shaders[SHADER_PHONG_LIGHTING],
                assets->shaders[SHADER_PHONG_LIGHTING].locs[LOC_VECTOR_VIEW],
                &game->camera->position, UNIFORM_VEC3);

            assets->sun = CreateSun(assets->shaders[SHADER_PHONG_LIGHTING],
                                    assets->sun.direction, assets->sun.ambient,
                                    assets->sun.diffuse);
            UpdateSunValue(assets->shaders[SHADER_PHONG_LIGHTING], assets->sun);

            for (int i = 0; i < assets->num_lights; i++) {
                Light self = assets->lights[i];
                Light light = CreateLight(
                    LIGHT_POINT, self.position, self.target, self.color,
                    assets->shaders[SHADER_PHONG_LIGHTING]);
                UpdateLightValues(assets->shaders[SHADER_PHONG_LIGHTING],
                                  light);
                assets->lights[i] = light;
            }

            for (int i = 0; i < assets->num_models; i++) {
                assets->models[i].materials[0].shader =
                    assets->shaders[SHADER_PHONG_LIGHTING];
            }

            for (int i = 0; i < game->ecs->num_entities; i++) {
                if (!is_ent_alive(game->ecs, i)) continue;

                EntStruct *self = get_ent(game->ecs, i);

                if (has_comp(game->ecs, self, Model)) {
                    get_comp(game->ecs, self, Model)->materials[0].shader =
                        game->assets->shaders[SHADER_PHONG_LIGHTING];
                }
            }
        }

        switch (game->scene) {
            case SCENE_GAME: {
                update_and_render_game_scene(game, ecs, particle_sys, map, gfx);
                break;
            }

            case SCENE_EDIT: {
                update_and_render_game_scene_with_editor(
                    game, ecs, particle_sys, map, gfx, editor);
                break;
            }

            case SCENE_MAIN_MENU: {
                update_and_render_menu_scene(menu_state, game, ecs,
                                             particle_sys, map, gfx);
                break;
            }
        }

        tmem_reset();
    }

    janet_deinit();
#endif

    CloseWindow();
}
