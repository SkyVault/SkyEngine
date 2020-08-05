#include "game.h"

static Game* static_ref = NULL;
static Ed* static_ed_ref = NULL;

Game* create_game(JanetTable* env) {

    Game* game = malloc(sizeof(Game));

    game->state = STATE_RUNNING;

    game->num_scenes = 0;
    game->scene = SCENE_MAIN_MENU;

    game->lock_camera = false;
    game->editor_open = false;
    game->env = env;
    game->noclip = false;

    game->particle_sys = create_particle_system(); 
    game->gfx = create_gfx_state();

    Camera camera = {0};
    camera.position = (Vector3){1.0f, ACTOR_HEIGHT, 1.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = FOV;
    camera.type = CAMERA_PERSPECTIVE;
    
    game->camera = malloc(sizeof(Camera));
    *game->camera = camera;

    game->assets = create_and_load_assets();
    game->ecs = create_ecs_world();

    Ed *editor = create_editor(); 
    static_ed_ref = editor;

    static_ref = game;
    return game;
}

void goto_scene_using_id(Game *self) {
    switch(self->scene) {
        case SCENE_MAIN_MENU: {
            MainMenuState *main_menu = malloc(sizeof(MainMenuState));
            main_menu->game = self;
            push_scene(self, (Scene){.func = main_menu_function, .data=(void*)main_menu});
            break;
        }

        case SCENE_GAME: {
            GameSceneState *game_scene = malloc(sizeof(GameSceneState));
            game_scene->game = self;
            game_scene->editor = NULL;
            push_scene(self, (Scene){.func = game_scene_function, .data=(void*)game_scene}); 
            break;
        }

        case SCENE_EDIT: {
            GameSceneState *game_scene = malloc(sizeof(GameSceneState));
            game_scene->game = self;
            game_scene->editor = static_ed_ref;
            push_scene(self, (Scene){.func = game_scene_function, .data=(void*)game_scene}); 
            break;
        }

        default:
            break;
    }
}

Game* game_get_static_ref() { return static_ref; }

void update_game(Game *game) {
    if (game->scene >= 0) {
        goto_scene_using_id(game); 
        game->scene = -1;
    }

    if (game->num_scenes > 0) {
        Scene *scene = &game->scenes[game->num_scenes - 1];
        scene->func((void*)scene->data, SCENE_STATE_UPDATE);
    }
}

void render_game(Game *game) {
    if (game->num_scenes > 0) {
        Scene *scene = &game->scenes[game->num_scenes - 1];
        scene->func((void*)scene->data, SCENE_STATE_RENDER);
    }
}

void push_scene(Game *self, Scene scene) {
    self->scenes[self->num_scenes] = scene;
    self->num_scenes++;
    scene.func((void*)scene.data, SCENE_STATE_LOAD);
}

void pop_scene(Game *self) {
    if (self->num_scenes > 0) {
        Scene *scene = &self->scenes[self->num_scenes - 1];
        scene->func((void*)scene->data, SCENE_STATE_DESTROY);
        self->num_scenes--;
    }
}    

void goto_scene(Game *self, Scene scene) {
    pop_scene(self);
    push_scene(self, scene);
}
