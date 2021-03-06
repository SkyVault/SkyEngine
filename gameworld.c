#include "gameworld.h"

#include "rlgl.h"
#include "utils.h"

bool add_actor_spawn(Region *self, int type, Vector3 position) {
  if (self->num_spawns < MAX_ACTOR_SPAWNS) {
    self->spawns[self->num_spawns++] = (ActorSpawn){
        .type = ACTOR_GIRL_1 + type,
        .position = position,
    };
    return true;
  }
  return false;
}

EntStruct *create_door(Vector3 pos, Game *game) {
  EcsWorld *ecs = game->ecs;
  EntId id = create_ent(ecs);
  EntStruct *self = get_ent(ecs, id);
  add_comp_obj(ecs, self, Physics, create_physics());
  add_comp(ecs, self, Transform, .translation = pos,
           .rotation = QuaternionIdentity());
  add_comp(ecs, self, Door, .facing = Facing_X, .state = Door_Closed,
           .timer = 0);
  get_comp(ecs, self, Physics)->friction = 0.2f;
  return self;
}

void create_x_door(Vector3 pos, Game *game) {
  EntStruct *self = create_door(pos, game);
  Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
  model.transform = MatrixMultiply(
      model.transform, MatrixScale(CUBE_SIZE, CUBE_SIZE, 0.5f * CUBE_SIZE));
  model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
  add_comp_obj(game->ecs, self, Model, model);
}

void create_z_door(Vector3 pos, Game *game) {
  EntStruct *self = create_door(pos, game);
  Model model = LoadModelFromMesh(game->assets->meshes[MESH_CUBE]);
  model.transform = MatrixMultiply(
      model.transform, MatrixScale(0.5f * CUBE_SIZE, CUBE_SIZE, CUBE_SIZE));
  model.materials[0].maps[MAP_DIFFUSE].color = BLUE;
  add_comp_obj(game->ecs, self, Model, model);
  get_comp(game->ecs, self, Door)->facing = Facing_Z;
}

bool is_number(const char *str) {
  size_t len = strlen(str);
  if (len == 0)
    return false;
  bool point = false;
  for (size_t i = 0; i < len; i++) {
    if (str[i] == '-' && i != 0)
      return false;
    if (str[i] == '.') {
      if (point) {
        return false;
      } else
        point = true;
      continue;
    }

    if (isdigit(str[i]) == false)
      return false;
  }
  return true;
}

GameWorld *create_game_world() {
  GameWorld *result = malloc(sizeof(GameWorld)); 
  memset(result->grid, 0, WORLD_WIDTH*WORLD_HEIGHT*sizeof(Region*)); 
  return result;
}

// Loads the entities and the gameworld regions and everything to start a new game.

// **** NEW GAME **** //
void load_game_world_new_start(GameWorld *self) {

}
// ******************** //

void zero_out_region(Region *self) {
  self->num_models = 0;
  self->num_props = 0;
  self->num_spawns = 0;
  self->num_exits = 0;
}

Node *load_node_tree(Assets *assets, JanetTable *node_table) {
  if (!node_table)
    return;

  Janet next_j = janet_table_get(node_table, janet_ckeywordv("next"));
  Janet child_j = janet_table_get(node_table, janet_ckeywordv("child"));
  Janet transform_j = janet_table_get(node_table, janet_ckeywordv("transform"));

  JanetTable *next = janet_unwrap_table(next_j);
  JanetTable *child = janet_unwrap_table(child_j);
  JanetArray *trans = janet_unwrap_array(transform_j);

  Node *node = create_node();
  if (!next && !child) {
    node->type = NODE_TYPE_EMPTY;
  } else {
    node->type = NODE_TYPE_MODEL;
  }

  if (janet_checktype(transform_j, JANET_ARRAY)) {
    Transform transform;
    transform.translation = (Vector3){janet_unwrap_number(trans->data[0]),
                                      janet_unwrap_number(trans->data[1]),
                                      janet_unwrap_number(trans->data[2])};
    transform.scale = (Vector3){janet_unwrap_number(trans->data[3]),
                                janet_unwrap_number(trans->data[4]),
                                janet_unwrap_number(trans->data[5])};
    transform.rotation = (Quaternion){janet_unwrap_number(trans->data[6]),
                                      janet_unwrap_number(trans->data[7]),
                                      janet_unwrap_number(trans->data[8]),
                                      janet_unwrap_number(trans->data[9])};
    node->transform = transform;
  }

  if (node->type == NODE_TYPE_MODEL) {
    Janet val = janet_table_get(node_table, janet_ckeywordv("model"));

    if (janet_checktype(val, JANET_STRING)) {
      const char *str = janet_unwrap_string(val);
      node->model = *assets_get_model(assets, str);
      for (size_t i = 0; i < strlen(str); i++)
        node->name[i] = str[i];
    } else {
      node->type = NODE_TYPE_EMPTY;
    }

    if (janet_checktype(child_j, JANET_TABLE)) {
      node->child = load_node_tree(assets, child);
      node->child->parent = node;
    }

    if (janet_checktype(next_j, JANET_TABLE)) {
      node->next = load_node_tree(assets, next);
      node->parent = NULL;
    }
  }

  return node;
}

Region *create_region_from_script(const char *path, Game *game) {
  Region *result = malloc(sizeof(Region));
  result->current_map = -1;
  result->scene_root = NULL;

  size_t psize = strlen(path);
  result->path.buff = malloc(sizeof(char) * psize + 1);
  for (int i = 0; i < psize; i++)
    result->path.buff[i] = path[i];

  result->path.len = psize;

  zero_out_region(result);

  result->num_models = game->assets->num_models;

  // Load the models and meshes
  for (int i = 0; i <= result->num_models; i++) {
    result->models[i] = game->assets->models[i];
  }

  // result->floor_tile_models[0] = game->assets->models[5];
  // result->floor_tile_models[0]

  load_region_from_script(result, path, game);

  return result;
}

void load_region_from_script(Region *result, const char *path, Game *game) {
  FILE *o = fopen(path, "r");

  result->num_models = game->assets->num_models;

  fseek(o, 0L, SEEK_END);
  size_t len = ftell(o);
  fseek(o, 0L, SEEK_SET);

  tstr s = talloc(len);
  fread(s, sizeof(char), len, o);

  Janet resultj;
  int res = janet_dostring(game->env, s, "main", &resultj);
  JanetTable *result_table = janet_unwrap_table(resultj);

  if (result_table == NULL)
    return;

  Shader *shader = &game->assets->shaders[SHADER_PHONG_LIGHTING];

  // Read the map size
  JanetArray *size_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("size")));

  JanetArray *start_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("start")));

  JanetArray *props_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("props")));

  JanetArray *lights_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("lights")));

  JanetArray *actors_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("actors")));

  JanetArray *exits_arr = janet_unwrap_array(
      janet_table_get(result_table, janet_ckeywordv("exits")));

  assert(size_arr->count == 2);

  Janet sun_dir_arr_val =
      janet_table_get(result_table, janet_ckeywordv("sun-direction"));

  if (janet_checktype(sun_dir_arr_val, JANET_ARRAY)) {
    JanetArray *arr = janet_unwrap_array(sun_dir_arr_val);

    const float x = janet_unwrap_number(arr->data[0]);
    const float y = janet_unwrap_number(arr->data[1]);
    const float z = janet_unwrap_number(arr->data[2]);

    game->assets->sun.direction = (Vector3){x, y, z};
    UpdateSunValue(*shader, game->assets->sun);
  }

  if (size_arr) {
    result->width = janet_unwrap_integer(size_arr->data[0]);
    result->height = janet_unwrap_integer(size_arr->data[1]);
  } else {
    printf("Region(%s)::load Missing size, going with the default (%d %d)",
           path, REGION_WIDTH, REGION_HEIGHT);
    result->width = REGION_WIDTH;
    result->height = REGION_HEIGHT;
  }

  if (start_arr) {
    result->player_x = janet_unwrap_number(start_arr->data[0]);
    result->player_z = janet_unwrap_number(start_arr->data[1]);
  } else {
    printf("Region(%s)::load Missing start, going with default (5 5)");
    result->player_x = 5.5f;
    result->player_z = 5.5f;
  }

  result->num_props = 0;

  result->num_exits = 0;

  game->assets->num_lights = 0;
  result->num_spawns = 0;

  if (props_arr != NULL) {
    for (int prop = 0; prop < props_arr->count; prop++) {
      JanetArray *prop_arr = janet_unwrap_array(props_arr->data[prop]);

      float rx = (float)janet_unwrap_number(prop_arr->data[0]);
      float ry = (float)janet_unwrap_number(prop_arr->data[1]);
      float rw = (float)janet_unwrap_number(prop_arr->data[2]);
      float rh = (float)janet_unwrap_number(prop_arr->data[3]);
      float x = (float)janet_unwrap_number(prop_arr->data[4]);
      float y = (float)janet_unwrap_number(prop_arr->data[5]);
      float z = (float)janet_unwrap_number(prop_arr->data[6]);
      float s = (float)janet_unwrap_number(prop_arr->data[7]);

      result->props[result->num_props++] = (Prop){
          .region = (Rectangle){rx, ry, rw, rh},
          .position = (Vector3){x, y, z},
          .scale = s,
      };
    }
  }

  if (lights_arr != NULL) {
    for (int i = 0; i < MAX_LIGHTS; i++) {
      game->assets->lights[i].enabled = false;

      if (i < lights_arr->count) {
        JanetArray *light_arr = janet_unwrap_array(lights_arr->data[i]);

        bool enabled = (bool)janet_unwrap_boolean(light_arr->data[0]);
        float x = (float)janet_unwrap_number(light_arr->data[1]);
        float y = (float)janet_unwrap_number(light_arr->data[2]);
        float z = (float)janet_unwrap_number(light_arr->data[3]);

        int r = (int)janet_unwrap_integer(light_arr->data[4]);
        int g = (int)janet_unwrap_integer(light_arr->data[5]);
        int b = (int)janet_unwrap_integer(light_arr->data[6]);

        game->assets->lights[i].position = (Vector3){x, y, z};
        game->assets->lights[i].color = (Color){r, g, b, 255};
        game->assets->lights[i].enabled = true;
        game->assets->num_lights++;
      }

      UpdateLightValues(*shader, game->assets->lights[i]);
    }

  } else {
    for (int i = 0; i < MAX_LIGHTS; i++) {
      UpdateLightValues(*shader, game->assets->lights[i]);
    }
  }

  if (actors_arr != NULL) {
    for (int actor_i = 0; actor_i < actors_arr->count; actor_i++) {
      JanetArray *actor_arr = janet_unwrap_array(actors_arr->data[actor_i]);

      int type = (int)janet_unwrap_integer(actor_arr->data[0]);
      float x = (float)janet_unwrap_number(actor_arr->data[1]);
      float y = (float)janet_unwrap_number(actor_arr->data[2]);
      float z = (float)janet_unwrap_number(actor_arr->data[3]);
      result->spawns[result->num_spawns++] = (ActorSpawn){
          .type = ACTOR_GIRL_1 + type,
          .position = (Vector3){x, y, z},
      };
      assemble(ACTOR_GIRL_1 + type, game, x, y, z, 0, 0);
    }
  }

  if (exits_arr != NULL && exits_arr != 0x1) {
    for (int exit_i = 0; exit_i < exits_arr->count; exit_i++) {
      JanetArray *exit_arr = janet_unwrap_array(exits_arr->data[exit_i]);

      int self_id = (int)janet_unwrap_integer(exit_arr->data[0]);

      float x = (float)janet_unwrap_number(exit_arr->data[1]);
      float y = (float)janet_unwrap_number(exit_arr->data[2]);
      float z = (float)janet_unwrap_number(exit_arr->data[3]);

      int dest_id = (int)janet_unwrap_integer(exit_arr->data[4]);

      const JanetString *dest_path = janet_unwrap_string(exit_arr->data[5]);

      add_exit(result, (Vector3){x, y, z}, self_id, dest_id, dest_path);
    }
  }

  Transform transform;
  transform.translation = (Vector3){2, 0, 2};
  transform.scale = Vector3One();
  transform.rotation = QuaternionIdentity();

  // Load node tree
  JanetTable *nodes_table = janet_unwrap_table(
      janet_table_get(result_table, janet_ckeywordv("root-node")));

  result->scene_root = load_node_tree(game->assets, nodes_table);

  fclose(o);
}

void update_region(Region *self, Game *game) {}

void render_region(Region *self, GfxState *gfx, Game *game) {
  rlEnableBackfaceCulling();
  rlEnableDepthTest();

  //  Rendering all of the props
  for (int pi = 0; pi < self->num_props; pi++) {
    draw_prop(gfx, game->assets, self->props[pi]);
  }

  draw_root_node(gfx, self->scene_root);
}

void reset_region_to_zero(Region *self, Game *game) {
  zero_out_region(self);

  if (self->scene_root) {
    delete_node_tree(self->scene_root);
    self->scene_root = NULL;
  }

  if (self->path.len > 0 && self->path.buff != NULL) {
    free(self->path.buff);
    self->path.buff = NULL;
    self->path.len = -1;
  }

  // Clean up the exits path string
  for (int i = 0; i < self->num_exits; i++)
    free(self->exits[i].dest_path);
}

Region *get_region(GameWorld *world, Vector3 translation) {
    
}

void reload_region(Region *self, Game *game) {
  tstr s = tstrf("%s", self->path);
  reset_region_to_zero(self, game);
  self->num_models = game->assets->num_models;
  load_region_from_script(self, s, game);
}

bool add_exit(Region *self, Vector3 position, int id, int dest_id,
              const char *dest_path) {
  if (self->num_exits < MAX_EXITS) {
    Exit result = (Exit){
        .id = id, .dest_id = dest_id, .dest_path = NULL, .position = position};

    size_t len = strlen(dest_path);
    result.dest_path = malloc(len + 1);
    result.dest_path[len] = '\0';
    for (size_t i = 0; i < len; i++)
      result.dest_path[i] = dest_path[i];

    self->exits[self->num_exits++] = result;

    return true;
  }

  return false;
}
