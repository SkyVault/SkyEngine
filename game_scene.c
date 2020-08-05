#include "game_scene.h"

void load(Game *game) {
  assemble(ACTOR_PLAYER, game, game->map->player_x, (ACTOR_HEIGHT - GLOBAL_SCALE),
            game->map->player_z, 0, 0);

  assemble(ACTOR_SCRALAPUS, game, 10, 10, 10, 0, 0); 
}

void update(Game *game, Ed *editor) {
    EcsWorld *ecs = game->ecs;
    ParticleSystem *particle_sys = game->particle_sys;
    Assets *assets = game->assets;
    Region *map = game->map;
    Camera *camera = game->camera;
    Shader *shader = &assets->shaders[SHADER_PHONG_LIGHTING];
  
    update_and_cleanup_ecs_world(ecs);
    update_particle_system(particle_sys);
  
    for (int i = 0; i < ecs->max_num_entities; i++) {
      if (!is_ent_alive(ecs, i))
        continue;
  
      update_billboard(ecs, i);
      update_player(ecs, assets, game, i);
      update_doors(ecs, i);
      update_behaviours(game, ecs, i);
      update_physics(map, ecs, game, i);
      update_timed_destroy(ecs, i);
    }
  
    update_region(map, game);

    if (editor)
      update_editor(editor, game->map, game);
  
    SetShaderValue(*shader, shader->locs[LOC_VECTOR_VIEW], &camera->position,
                   UNIFORM_VEC3);
  
    Matrix view = GetCameraMatrix(*camera);
    SetShaderValueMatrix(*shader, shader->locs[LOC_MATRIX_VIEW], view);
}

void render(Game *game, Ed *editor) {
    EcsWorld *ecs = game->ecs;
    ParticleSystem *particle_sys = game->particle_sys;
    Assets *assets = game->assets;
    Region *map = game->map;
    Camera *camera = game->camera;
    GfxState *gfx = game->gfx;
    Shader *shader = &assets->shaders[SHADER_PHONG_LIGHTING];

    BeginDrawing();
    ClearBackground((Color){100, 210, 255, 255});

    begin_rendering(gfx);
        BeginMode3D(*camera);
  
        rlViewport(0, 0, gfx->render_texture.texture.width,
                   gfx->render_texture.texture.height);
  
        render_region(map, gfx, game);
  
        for (int i = 0; i < ecs->max_num_entities; i++) {
            if (!is_ent_alive(ecs, i))
                continue;
  
          //TODO(Dustin): Move this from the draw function
  
            draw_billboard_ent(gfx, camera, ecs, i);
            draw_models(gfx, ecs, i);
        }
  
        render_particle_system(particle_sys);
  
        glDisable(GL_CULL_FACE);
        DrawModel(game->skybox, (Vector3){0, 0, 0}, 100.0f, WHITE);
        glEnable(GL_CULL_FACE);
  
        // Do the final draw to the screen
        flush_graphics(gfx, camera);
  
        if (editor) 
            render_editor(editor, gfx, map, game);
  
        EndMode3D();
    end_rendering(gfx);
  
    draw_final_texture_to_screen(gfx);

    update_gui();
    draw_player_gui(game, map);
    if (editor)
        render_editor_ui(editor, gfx, map, game);
    flush_gui_3d_graphics(gfx);
  
    DrawFPS(10, 10);
    EndDrawing();
}

void destroy() {

}

void game_scene_function(void *data, int state) {
    GameSceneState *self = (GameSceneState*) data;
    if (!self) return;

    switch(state) {
        case SCENE_STATE_LOAD: load(self->game); break;
        case SCENE_STATE_UPDATE: update(self->game, self->editor); break;
        case SCENE_STATE_RENDER: render(self->game, self->editor); break;
        case SCENE_STATE_DESTROY: destroy(); break;
    }
}
