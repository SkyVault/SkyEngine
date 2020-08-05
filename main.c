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
#include "main_menu.h"

// #define PLATFORM_DESKTOP

#define BUFFSIZE (512)

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else
#define GLSL_VERSION 100
#endif

void custom_logger(int msg_type, const char *text, va_list args) {
  // printf(text, args);
  // printf("\n");
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  // SetTraceLogLevel(0);
  // SetTraceLogCallback(custom_logger);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DevWindow");

  SetTargetFPS(60);
  SetExitKey(0);

  // glew
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }

  // Init scripting language
  janet_init();

  JanetTable *env = janet_core_env(NULL);
  janet_dostring(env, "(print `Initializing Janet`)", "main", NULL);

  assemblers_init();

  Game *game = create_game(env);

  Assets *assets = game->assets;
  assets_load_scripts(game->assets, env);

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

  init_gui();

  game->map = create_region_from_script("resources/maps/map1.janet", game);
  Region *map = game->map;

  GameWorld *world = create_game_world(); 
  load_game_world_new_start(world);

  rlEnableBackfaceCulling();
  rlEnableDepthTest();
  glClearDepth(1.0f);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

  while (!WindowShouldClose() && game->state == STATE_RUNNING) {
    update_game(game); 
    update_assets(assets); 
    render_game(game);
    tmem_reset();
  }

  janet_deinit();

  CloseWindow();
}
