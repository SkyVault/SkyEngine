#include "raymath.h"
#include "rlgl.h"
#include "raylib.h"

#include <stdio.h>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#include "physics.h"
#include "models.h"
#include "player.h"
#include "ecs.h"
#include "game.h"
#include "map.h"
#include "assets.h"
#include "editor.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else
#define GLSL_VERSION 100
#endif

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT ((SCREEN_WIDTH) * (160.0 / 240.0))
#define MAX_COLUMNS (20)

void custom_logger(int msg_type, const char *text, va_list args) {
    // TODO(Dustin): Route this information somewhere
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT); 
    //SetTraceLogLevel(0);
    //SetTraceLogCallback(custom_logger);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DevWindow");
    SetTargetFPS(60);

    //glBlendEquation(GL_FUNC_ADD);
    //glBendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    rlEnableBackfaceCulling();
    rlEnableDepthTest();

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 10.0f, 1.0f, 20.0f };
    camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 75.0f;
    camera.type = CAMERA_PERSPECTIVE;

    Assets* assets = create_and_load_assets();
    EcsWorld* ecs = create_ecs_world();

    Game* game = create_game(assets, &camera, ecs);
    GfxState* gfx = create_gfx_state();

    Shader* shader = &assets->shaders[SHADER_PHONG_LIGHTING];
    shader->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");
    shader->locs[LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");

    int ambientLoc = GetShaderLocation(*shader, "ambient");
    SetShaderValue(*shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f}, UNIFORM_VEC4);

    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -10, 0, -10 }, (Vector3){0}, WHITE, *shader);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        UpdateLightValues(*shader, lights[i]);
    }

    // Load the skybox
    game->skybox = LoadModelFromMesh(assets->meshes[MESH_SKYBOX]);
    game->skybox.materials[0].shader = assets->shaders[SHADER_SKYBOX];
    SetShaderValue(
        game->skybox.materials[0].shader,
        GetShaderLocation(game->skybox.materials[0].shader,
                          "environment_map"),
        (int[1]){ MAP_CUBEMAP }, UNIFORM_INT);

    Shader* cubemap_shader = &assets->shaders[SHADER_CUBEMAP];
    SetShaderValue(*cubemap_shader, GetShaderLocation(*cubemap_shader, "equirectagular_map"), (int[1]){0}, UNIFORM_INT);

    Texture2D skybox_texture = LoadTexture("resources/skybox_1.hdr");
    game->skybox.materials[0].maps[MAP_CUBEMAP].texture = GenTextureCubemap(*cubemap_shader, skybox_texture, 512);

    //UnloadTexture(skybox_texture);
    //UnloadShader(*cubemap_shader);

    Map* map = load_map(0, game);

    // Create player
    {
        EntId player_id = create_ent(ecs);
        EntStruct* player = get_ent(ecs, player_id);

        add_comp(ecs, player, Transform, .translation=(Vector3){10.0f, 1.0f, 20.0f}); 
		add_comp(ecs, player, Player, .n = 0); 
        add_comp_obj(ecs, player, Physics, create_physics());
    }

#if defined _DEBUG
    Ed* editor = create_editor();
#endif

    while (!WindowShouldClose() && game->state == STATE_RUNNING) {
        update_game(game);

        for (int i = 0; i < ecs->max_num_entities; i++) {
            if (!is_ent_alive(ecs, i)) continue;
            update_billboard(ecs, i);
            update_player(ecs, assets, game, i);
            update_doors(ecs, i);
        }

        update_map(map, game);

#if defined _DEBUG
        update_editor(editor, map, game);
#endif

        for (int i = 0; i < MAX_LIGHTS; i++) {
            lights[i].position = camera.position;
            UpdateLightValues(*shader, lights[i]);
        }

        SetShaderValue(*shader, shader->locs[LOC_VECTOR_VIEW], &camera.position, UNIFORM_VEC3);
        
        BeginDrawing();
            ClearBackground((Color){100, 210, 255, 255}); 

            if (IsKeyDown(KEY_TAB)) {
                rlEnableWireMode();
            } else {
                rlDisableWireMode();
            }

            BeginMode3D(camera);

                draw_map(map, game);

                for (int i = 0; i < ecs->max_num_entities; i++) {
                    if (!is_ent_alive(ecs, i)) continue;
                    draw_billboard(gfx, &camera, ecs, i);
                    update_physics(map, ecs, game, i);
                    draw_models(gfx, ecs, i);
                }

				DrawModel(game->skybox, (Vector3){0, 0, 0}, 100.0f, WHITE);


                // Do the final draw to the screen
                flush_graphics(gfx, &camera); 

                render_editor(editor, map, game);

            EndMode3D();

#if defined _DEBUG
			render_editor_ui(editor, map, game);
#endif

        EndDrawing();
    }

    CloseWindow();
}
