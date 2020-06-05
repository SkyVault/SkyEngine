#include <GL/glew.h>
#include <janet.h>
#include <stdio.h>

#include "assembler.h"
#include "assets.h"
#include "behaviours.h"
#include "constants.h"
#include "dict.h"
#include "ecs.h"
#include "editor.h"
#include "game.h"
#include "gui.h"
#include "map.h"
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
#define BENIS_VERSION_PATCH "8"
#define BENIS_VERSION \
    BENIS_VERSION_MAJOR "." BENIS_VERSION_MINOR "." BENIS_VERSION_PATCH

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT ((int)((SCREEN_WIDTH) * (160.0 / 240.0)))
#define MAX_COLUMNS (20)

#define MAIN_SHADER SHADER_PHONG_LIGHTING

void custom_logger(int msg_type, const char *text, va_list args) {
    printf(text, args);
    printf("\n");
}

float timer = 0.0f;

void update_and_render_menu_scene(Game *game, EcsWorld *ecs,
                                  ParticleSystem *particle_sys, Map *map,
                                  GfxState *gfx) {
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

    BeginDrawing();
    ClearBackground((Color){10, 20, 21, 255});

    Texture2D t = assets->textures[TEX_GIRL_1];

    float x = GetScreenWidth() - 20;
    float y = GetScreenHeight() - 20;

    // DrawTexturePro(t, (Rectangle){0, 0, t.width, t.height},
    //               (Rectangle){x, y, 500, 500}, (Vector2){250, 500}, rot,
    //               WHITE);

    const float shw = GetScreenWidth() / 2;
    const float shh = GetScreenHeight() / 2;

    DoCenterXLabel(100, GetScreenWidth(), 30, 50, "Benis Shooter 3D");

    DoCenterXLabel(100, GetScreenWidth() + cosf(GetTime() * 2) * 50.0f, 70, 30,
                   TextFormat("Edit Version: %s", BENIS_VERSION));

    // Buttons
    int id = 0;
    const float btn_w = GetScreenWidth() / 2.2f;
    const float btn_h = 80.0f;
    const float margin = 30.0f;
    const float tot_h = (btn_h + margin) * 4;
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

#define DO_BTN(txt)                                                    \
    DoBtn(id, shw + xx - btn_w / 2 - mo / 2,                           \
          (btn_h + margin) * id + tot_h / 2 + oy - mo / 2, btn_w + mo, \
          btn_h + mo, txt)

#define DO_SCALING() \
    if (Hot(id)) scaler_t += GetFrameTime() * 2.0f

    bool shrink = true;
    static bool do_controls_modal = false;

    {
        DO_SCALING();
        GEASE(1.0f);
        if (DO_BTN("Start New Game :)")) {
            game->scene = SCENE_GAME;
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

    {
        DO_SCALING();
        GEASE(1.2f);
        if (DO_BTN("Controls") && !do_controls_modal) {
            do_controls_modal = true;
        }
        if (Hot(id)) shrink = false;
        ++id;
    }

    static bool do_exit_modal = false;

    {
        DO_SCALING();
        GEASE(1.3f);
        if (DO_BTN("Exit :(")) do_exit_modal = true;
        if (Hot(id)) shrink = false;
        ++id;
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

        if (DoBtn(id++, GetScreenWidth() / 2, 400, 30, 30, "X")) {
            do_controls_modal = false;
        }

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

void update_and_render_game_scene(Game *game, EcsWorld *ecs,
                                  ParticleSystem *particle_sys, Map *map,
                                  GfxState *gfx, Ed *editor) {
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
        update_behaviours(ecs, i);
        update_timed_destroy(ecs, i);
        update_physics(map, ecs, game, i);
    }

    update_map(map, game);

    // #if defined _DEBUG
    update_editor(editor, map, game);
    // #endif

    SetShaderValue(*shader, shader->locs[LOC_VECTOR_VIEW], &camera->position,
                   UNIFORM_VEC3);

    BeginDrawing();
    ClearBackground((Color){100, 210, 255, 255});

    if (IsKeyDown(KEY_TAB)) {
        rlEnableWireMode();
    } else {
        rlDisableWireMode();
    }

    BeginMode3D(*camera);

    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());

    render_map(map, gfx, game);

    for (int i = 0; i < ecs->max_num_entities; i++) {
        if (!is_ent_alive(ecs, i)) continue;
        draw_billboard(gfx, camera, ecs, i);
        draw_models(gfx, ecs, i);
    }

    render_particle_system(particle_sys);

    glDisable(GL_CULL_FACE);
    DrawModel(game->skybox, (Vector3){0, 0, 0}, 100.0f, WHITE);
    glEnable(GL_CULL_FACE);

    // Do the final draw to the screen
    flush_graphics(gfx, camera);

    // #if defined _DEBUG
    render_editor(editor, gfx, map, game);
    // #endif

    EndMode3D();

    UpdateGui();
    draw_player_gui(game, map);

    // #if defined _DEBUG
    render_editor_ui(editor, gfx, map, game);
    // #endif

    DrawFPS(10, 10);

    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    SetExitKey(-1);
    // SetTraceLogLevel(0);
    // SetTraceLogCallback(custom_logger);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DevWindow");
    SetTargetFPS(86);

    // glew
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    // Init scripting language
    janet_init();

    JanetTable *env = janet_core_env(NULL);
    janet_dostring(env, "(print `Initializing Janet`)", "main", NULL);

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = {0};
    camera.position = (Vector3){1.0f, ACTOR_HEIGHT, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = FOV;
    camera.type = CAMERA_PERSPECTIVE;

    Assets *assets = create_and_load_assets();
    EcsWorld *ecs = create_ecs_world();

    Game *game = create_game(assets, &camera, ecs, env);
    GfxState *gfx = create_gfx_state();

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

    game->map = create_map_from_script("resources/maps/edit.janet", game);
    Map *map = game->map;

    assemble(ACTOR_PLAYER, game, map->player_x, (ACTOR_HEIGHT - GLOBAL_SCALE),
             map->player_z, 0, 0);

    Model terrain_m = LoadModel("resources/models/terrain.obj");
    terrain_m.materials[0].maps[MAP_DIFFUSE].texture =
        game->assets->textures[TEX_GRASS_1];
    terrain_m.materials[0].shader = assets->shaders[SHADER_PHONG_LIGHTING];

    EntStruct *terrain = create_and_get_ent(ecs);
    add_comp_obj(ecs, terrain, Model, terrain_m);
    add_comp(ecs, terrain, Transform,
             .translation =
                 (Vector3){(32 * CUBE_SIZE) / 2, 1.49, (32 * CUBE_SIZE / 2)},
             .rotation = QuaternionIdentity(),
             .scale = (Vector3){1.0, 1.0, 1.0});

    // #if defined _DEBUG
    Ed *editor = create_editor();
    // #endif

    rlEnableBackfaceCulling();
    rlEnableDepthTest();

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    // glBlendFunc(GL_ONE, GL_ONE);

    while (!WindowShouldClose() && game->state == STATE_RUNNING) {
        map = game->map;
        switch (game->scene) {
            case SCENE_GAME: {
                update_and_render_game_scene(game, ecs, particle_sys, map, gfx,
                                             editor);
                break;
            }

            case SCENE_MAIN_MENU: {
                update_and_render_menu_scene(game, ecs, particle_sys, map, gfx);
                break;
            }
        }

        tmem_reset();
    }

    janet_deinit();
    CloseWindow();
}
