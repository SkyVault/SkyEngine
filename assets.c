#include "assets.h"

#define SUN_DIRECTION ((Vector3){10.2f, -20.0f, 10.3})

Shader hotload_shader(Assets *self, const char *path_vs, const char *path_fs,
                      int id) {
  Shader result = LoadShader(path_vs, path_fs);

  self->shaders_hotload[id].hotload = true;
  self->shaders_hotload[id].paths[SHADER_VERTEX] = path_vs;
  self->shaders_hotload[id].paths[SHADER_FRAGMENT] = path_fs;
  self->shaders_hotload[id].last_time_modified[SHADER_VERTEX] =
      GetFileModTime(path_vs);
  self->shaders_hotload[id].last_time_modified[SHADER_FRAGMENT] =
      GetFileModTime(path_fs);

  return result;
}

Assets *create_and_load_assets(void) {
  Assets *ass = malloc(sizeof(Assets));

  ass->models_dict = malloc(sizeof(map_void_t));
  map_init(ass->models_dict);

  ass->prefabs_dict = malloc(sizeof(map_void_t));
  map_init(ass->prefabs_dict);

  for (int i = 0; i < SCRIPTS_NUM_SCRIPTS; i++) {
    ass->scripts[i] = NULL;
  }

  for (int i = 0; i < SHADER_NUM_SHADERS; i++) {
    ass->shaders_hotload[i].hotload = false;
    ass->shaders_hotload[i].just_hotloaded = false;
  }

  ass->meshes[MESH_CUBE] = GenMeshCube(1, 1, 1);
  ass->meshes[MESH_SKYBOX] = GenMeshCube(200, 200, 200);
  ass->meshes[MESH_PLANE] = GenMeshPlane(1, 1, 10, 10);

  ass->textures[TEX_WALL_1] = LoadTexture("resources/wall_1.png");
  ass->textures[TEX_WALL_2] = LoadTexture("resources/wall_2.png");
  ass->textures[TEX_WALL_3] = LoadTexture("resources/wall_3.png");
  ass->textures[TEX_WALL_4] = LoadTexture("resources/wall_4.png");
  ass->textures[TEX_WALL_5] = LoadTexture("resources/wall_5.png");
  ass->textures[TEX_WALL_6] = LoadTexture("resources/wall_6.png");

  ass->textures[TEX_WALL_ATLAS] = LoadTexture("resources/textures/blocks.png");
  ass->textures[TEX_PROPS] = LoadTexture("resources/props.png");
  ass->textures[TEX_CHAINLINK_FENCE] = LoadTexture("resources/chainlink.png");
  ass->textures[TEX_FLOOR_1] = LoadTexture("resources/floor_1.png");
  ass->textures[TEX_CHAR_1] = LoadTexture("resources/char.png");
  ass->textures[TEX_SALAMI] = LoadTexture("resources/salami.png");
  ass->textures[TEX_PINEAPPLE] = LoadTexture("resources/pineapple.png");
  ass->textures[TEX_ORANGE] = LoadTexture("resources/orange.png");
  ass->textures[TEX_GIRL_1] = LoadTexture("resources/girls/girl_1.png");
  ass->textures[TEX_GIRL_2] = LoadTexture("resources/girls/girl_2.png");
  ass->textures[TEX_GIRL_3] = LoadTexture("resources/girls/girl_3.png");
  ass->textures[TEX_GIRL_4] = LoadTexture("resources/girls/girl_4.png");
  ass->textures[TEX_GRASS_1] = LoadTexture("resources/textures/grass_1.png");

  ass->textures[TEX_SCRALAPUS] = LoadTexture("resources/textures/scralapus.png");
  ass->textures[TEX_BRICKS] = LoadTexture("resources/textures/brickTexture.png");

  ass->textures[TEX_BARREL] = LoadTexture("resources/textures/barrel.png");

  ass->shaders[SHADER_PHONG_LIGHTING] =
      hotload_shader(ass, "resources/phong_vs.glsl", "resources/phong_fs.glsl",
                     SHADER_PHONG_LIGHTING);

  ass->shaders[SHADER_SKYBOX] =
      LoadShader("resources/skybox_vs.glsl", "resources/skybox_fs.glsl");
  ass->shaders[SHADER_CUBEMAP] =
      LoadShader("resources/cubemap_vs.glsl", "resources/cubemap_fs.glsl");

  ass->fonts[FONT_MAIN_FONT] = LoadFont("resources/alagard_font.png");

  Shader *shader = &ass->shaders[SHADER_PHONG_LIGHTING];

  shader->locs[LOC_VECTOR_VIEW] = GetShaderLocation(*shader, "viewPos");

  shader->locs[LOC_MATRIX_VIEW] = GetShaderLocation(*shader, "matView");
  shader->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*shader, "matModel");

  ass->sun = CreateSun(*shader, SUN_DIRECTION, (Color){70, 70, 70, 255},
                       (Color){255, 255, 255, 255});

  // Initialize the lights
  for (int i = 0; i < MAX_LIGHTS; i++) {
    ass->lights[i] =
        CreateLight(LIGHT_POINT, Vector3Zero(), Vector3Zero(), WHITE, *shader);
    ass->lights[i].enabled = false;
    UpdateLightValues(*shader, ass->lights[i]);
  }

  for (int i = 0; i < TEX_NUM_TEXTURES; i++) {
    SetTextureFilter(ass->textures[i], FILTER_POINT);
    SetTextureWrap(ass->textures[i], WRAP_REPEAT);
  }

  ass->num_models = 6;
  ass->models = malloc(sizeof(Model) * ass->num_models);

  Model default_wall_1 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_1.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_1];
  default_wall_1.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  Model default_wall_2 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_2.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_2];
  default_wall_2.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  Model default_wall_3 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_3.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_3];
  default_wall_3.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  Model default_wall_4 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_4.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_4];
  default_wall_4.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  Model default_wall_5 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_5.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_5];
  default_wall_5.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  Model default_wall_6 = LoadModelFromMesh(ass->meshes[MESH_CUBE]);
  default_wall_6.materials[0].maps[MAP_DIFFUSE].texture =
      ass->textures[TEX_WALL_6];
  default_wall_6.materials[0].shader = ass->shaders[SHADER_PHONG_LIGHTING];

  ass->models[0] = default_wall_1;
  ass->models[1] = default_wall_2;
  ass->models[2] = default_wall_3;
  ass->models[3] = default_wall_4;
  ass->models[4] = default_wall_5;
  ass->models[5] = default_wall_6;

  assets_load_prefabs(ass);
  assets_load_assets_from_prefabs(ass);

  return ass;
}

bool at_eof(char** it, const char* end) {
    return (*(it)) >= end;
}

void skip_ws(char** it, const char* end) {
    while (!at_eof(it, end) && isspace((*(*it)))) {
        (*it)++;
    }
}

void inc(char** it) { (*(it))++;       }
char chr(char** it) { return (*(*it)); }

const char* get_word(char** it, const char* end) {
    char* start = (*it);
    while (!at_eof(it, end) && chr(it) != '[' && chr(it) != ']' && !isspace(chr(it))) {
        inc(it);
    }
    return FormatText("%.*s", (int)((*it)-start), start);
}

bool verify_prefab(Assets *self, Prefab* prefab) {
    return true;
}

void parse_prefab(Assets *self, char** it, const char* end) {
    skip_ws(it, end);
    if (at_eof(it, end)) return;

    const char* name = get_word(it, end);

    Prefab* prefab = malloc(sizeof(Prefab));
    *prefab = (Prefab) {
        .name = malloc(strlen(name) + 1),
        .model = NULL,
        .texture = NULL,
        .tags = 0,
    };
    strcpy(prefab->name, name);
    prefab->name[strlen(name)] = '\0';

    skip_ws(it, end);

    if (chr(it) == '[') {
        inc(it);
        while (!at_eof(it, end)) {
            skip_ws(it, end);
            const char *key = get_word(it, end);
            if (strlen(key) == 0) break;
            skip_ws(it, end);
            const char *value = get_word(it, end);
            if (strlen(value) == 0) break;

            if (strcmp(key, "model") == 0) {
                prefab->model = malloc(strlen(value) + 1);
                strcpy(prefab->model, value);
                prefab->model[strlen(value)] = '\0';
            } else if (strcmp(key, "texture") == 0) {
                prefab->texture = malloc(strlen(value) + 1);
                strcpy(prefab->texture, value);
                prefab->texture[strlen(value)] = '\0';
            } else {
                printf("Prefab::Error:: unknown prefab key [%s]\n", key);
                return;
            }

            skip_ws(it, end);
            if (chr(it) == ']') {
                inc(it);
                break;
            }
        }
    }

    if (verify_prefab(self, prefab)) {
        map_set(self->prefabs_dict, prefab->name, prefab);
        printf("Loaded Prefab: [%s]\n", prefab->name);
    } else {
        free(prefab->name);
        free(prefab->model);
        free(prefab->texture);
        free(prefab);
        return;
    }
}

void assets_load_prefabs(Assets *self) {
    char* models_prefab_text = LoadFileText("resources/prefabs/models.prefab");

    if (models_prefab_text == NULL) {
        printf("Failed to read prefab file\n");
        return;
    }

    char *it = models_prefab_text;
    char *end = it + strlen(models_prefab_text);

    int n = 0;
    while (!at_eof(&it, end)) {
        parse_prefab(self, &it, end);

        if (n++ > 100) break;
    }

    free(models_prefab_text);
}

void assets_load_assets_from_prefabs(Assets *self) {
    // for each prefab, check to see if the resource is already
    // loaded, if it is loaded then don't reload

    const char *key;
    map_iter_t iter = map_iter(self->prefabs_dict);
    while ((key = map_next(self->prefabs_dict, &iter))) {
        Prefab *prefab = (*(Prefab **)map_get(self->prefabs_dict, key));
        if (prefab == NULL) continue;
        
        if (map_get(self->models_dict, prefab->name) == NULL) {
            //TODO(Dustin): Handle primitive objects
            Model model = LoadModel(FormatText("resources/models/%s.obj", prefab->model));
            if (prefab->texture) {
                model.materials[0].maps[MAP_DIFFUSE].texture =
                    LoadTexture(FormatText("resources/textures/%s.png", prefab->texture));
                SetTextureFilter(model.materials[0].maps[MAP_DIFFUSE].texture, FILTER_POINT);
            }
            model.materials[0].shader = self->shaders[SHADER_PHONG_LIGHTING];
            map_set(self->models_dict, prefab->name, alloc_model_from(model));
        } else {
            // already loaded
            printf("Model [%s] is already loaded, skipping...\n", prefab->name);
        }
    }
}

Model *assets_get_model(Assets *self, const char *name) {
  Model **model_ref = map_get(self->models_dict, name);
  if (model_ref == NULL)
    return &self->models[0];
  return *model_ref;
}

void assets_load_scripts(Assets *self, JanetTable *env) {
  Janet result;
  janet_dostring(env, LoadText("resources/scripts/simple_zombie_ai.janet"),
                 "main", &result);

  JanetType type = janet_type(result);

  if (janet_checktype(result, JANET_FUNCTION)) {
    int lock = janet_gclock();
    JanetFunction *func = janet_unwrap_function(result);
    janet_gcunlock(lock);

    self->scripts[SCRIPTS_BASIC_ZOMBIE_AI] = func;
  } else {
    printf("Not a function\n");
  }
}

void update_assets(Assets *self) {
  for (int id = 0; id < SHADER_NUM_SHADERS; id++) {
    struct Hotload *hot = &self->shaders_hotload[id];

    if (hot->hotload == false)
      continue;

    hot->just_hotloaded = false;

    long times[] = {GetFileModTime(hot->paths[SHADER_VERTEX]),
                    GetFileModTime(hot->paths[SHADER_FRAGMENT])};

    for (int j = 0; j < 2; j++) {
      // printf("times fs %d != %d\n", times[1],
      // hot->last_time_modified[1]);
      if (times[j] != hot->last_time_modified[j]) {
        printf("Hotloading shader [%s] [%s]\n", hot->paths[SHADER_VERTEX],
               hot->paths[SHADER_FRAGMENT]);
        UnloadShader(self->shaders[id]);
        self->shaders[id] = hotload_shader(self, hot->paths[SHADER_VERTEX],
                                           hot->paths[SHADER_FRAGMENT], id);

        hot->just_hotloaded = true;
      }
    }
  }
}
