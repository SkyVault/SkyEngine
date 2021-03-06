#include "player.h"

#define PLAYER_MOVEMENT_SENSITIVITY (1.0f)

#define CAMERA_MOUSE_MOVE_SENSITIVITY 0.003f
#define CAMERA_MOUSE_SCROLL_SENSITIVITY 1.5f
#define CAMERA_FIRST_PERSON_MOUSE_SENSITIVITY 0.003f
#define CAMERA_FIRST_PERSON_FOCUS_DISTANCE 25.0f
#define CAMERA_FIRST_PERSON_MIN_CLAMP 89.0f
#define CAMERA_FIRST_PERSON_MAX_CLAMP -89.0f
#define CAMERA_FREE_SMOOTH_ZOOM_SENSITIVITY 0.05f
#define CAMERA_FREE_PANNING_DIVIDER 5.1f
#define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER 5.0f
#define CAMERA_FIRST_PERSON_STEP_DIVIDER 30.0f
#define CAMERA_FIRST_PERSON_WAVING_DIVIDER 200.0f

#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

void set_camera_mode(Camera camera, int mode) {
  Vector3 v1 = camera.position;
  Vector3 v2 = camera.target;

  float dx = v2.x - v1.x;
  float dy = v2.y - v1.y;
  float dz = v2.z - v1.z;

  CAMERA.targetDistance = sqrtf(dx * dx + dy * dy + dz * dz);

  // Camera angle calculation
  CAMERA.angle.x = atan2f(
      dx,
      dz); // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
  CAMERA.angle.y =
      atan2f(dy, sqrtf(dx * dx + dz * dz)); // Camera angle in plane XY (0
                                            // aligned with X, move positive CW)

  CAMERA.playerEyesPosition = camera.position.y;

  // Lock cursor for first person and third person cameras
  if ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON))
    DisableCursor();
  else
    EnableCursor();

  CAMERA.mode = mode;
}

void throw_orange(EcsWorld *ecs, Vector3 pos, float angle, float yaw,
                  Assets *ass) {
  EntId bullet_id = create_ent(ecs);
  EntStruct *bullet = get_ent(ecs, bullet_id);

  pos.x += cosf(angle);
  pos.y -= 0.5f;
  pos.z += sinf(angle);

  add_comp(ecs, bullet, Transform, .translation = pos);
  add_comp_obj(ecs, bullet, Physics, create_physics());
  add_comp(ecs, bullet, Actor, .type = ACTOR_TOSSED_ORANGE, .state = IDLE);

  add_comp(ecs, bullet, Billboard, .texture = ass->textures[TEX_ORANGE],
           .material = (Material){0}, .scale = GLOBAL_SCALE * 0.5f);

  add_comp(ecs, bullet, TimedDestroy, .time_left = 4.0f, .done = false);
  add_comp(ecs, bullet, PlayerHit, .damage = 1.0f);

  Physics *physics = get_comp(ecs, bullet, Physics);
  physics->velocity.x = cosf(angle) * 800.0f * GetFrameTime();
  physics->velocity.z = sinf(angle) * 800.0f * GetFrameTime();
  physics->velocity.y = 15.0f * yaw + 900.0f * GetFrameTime();
  physics->friction = 0.5f;
  physics->gravity_scale = 1.0f;
  physics->bounce_factor = 0.8f;
}

void throw_pineapple(EcsWorld *ecs, Vector3 pos, float angle, float yaw,
                     Assets *ass) {
  EntId bullet_id = create_ent(ecs);
  EntStruct *bullet = get_ent(ecs, bullet_id);

  pos.x += cosf(angle);
  pos.y -= 0.5f;
  pos.z += sinf(angle);

  add_comp(ecs, bullet, Transform, .translation = pos);
  add_comp_obj(ecs, bullet, Physics, create_physics());
  add_comp(ecs, bullet, Actor, .type = ACTOR_PINEAPPLE_BOMB, .state = IDLE);

  add_comp(ecs, bullet, Billboard, .texture = ass->textures[TEX_PINEAPPLE],
           .material = (Material){0}, .scale = GLOBAL_SCALE * 0.5f);

  add_comp(ecs, bullet, TimedDestroy, .time_left = 4.0f, .done = false);
  add_comp(ecs, bullet, PlayerHit, .damage = 1.0f);

  Physics *physics = get_comp(ecs, bullet, Physics);
  physics->velocity.x = cosf(angle) * 600.0f * GetFrameTime();
  physics->velocity.z = sinf(angle) * 600.0f * GetFrameTime();
  physics->velocity.y = 15.0f * yaw + 980.0f * GetFrameTime();
  physics->friction = 0.5f;
  physics->gravity_scale = 1.0f;
  physics->bounce_factor = 0.8f;
}

void update_player(EcsWorld *ecs, Assets *ass, Game *game, EntId id) {
  EntStruct *self = get_ent(ecs, id);

  if (!has_comp(ecs, self, Player) || !has_comp(ecs, self, Transform) ||
      !has_comp(ecs, self, Physics))
    return;

  Camera *camera = game->camera;

  Transform *transform = get_comp(ecs, self, Transform);
  Physics *physics = get_comp(ecs, self, Physics);
  Player *player = get_comp(ecs, self, Player);

  const float angle = PI * 2 - CAMERA.angle.x - PI / 2;

  player->rotation = angle;

  // Shoot
  if (!game->noclip) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      throw_orange(ecs, transform->translation, angle, CAMERA.angle.y, ass);
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
      throw_pineapple(ecs, transform->translation, angle, CAMERA.angle.y, ass);
    }

    if (IsKeyDown(KEY_SPACE) && physics->grounded) {
        physics->velocity.y = PLAYER_START_JUMP_STRENGTH * GetFrameTime();
        physics->grounded = false;
    } 
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    game->lock_camera = !game->lock_camera; 
    SetMousePosition(GetScreenWidth()/2, GetScreenHeight()/2); 
  }

  static int first = 0;

  if (!first) {
    set_camera_mode(*camera, CAMERA_FIRST_PERSON);
    first = 1;
  }

  static int swingCounter = 0; // Used for 1st person swinging movement

  // Mouse movement detection
  Vector2 mousePosition = GetMousePosition();

  bool direction[6] = {IsKeyDown(CAMERA.moveControl[MOVE_FRONT]),
                       IsKeyDown(CAMERA.moveControl[MOVE_BACK]),
                       IsKeyDown(CAMERA.moveControl[MOVE_RIGHT]),
                       IsKeyDown(CAMERA.moveControl[MOVE_LEFT]),
                       IsKeyDown(CAMERA.moveControl[MOVE_UP]),
                       IsKeyDown(CAMERA.moveControl[MOVE_DOWN])};

  Vector2 mousePositionDelta = {0.0f, 0.0f};
  mousePositionDelta.x = mousePosition.x - GetScreenWidth() / 2.0f;
  mousePositionDelta.y = mousePosition.y - GetScreenHeight() / 2.0f;

  //@debug
  if (IsKeyDown(KEY_Z))
    printf("delta; %f %f\n", mousePositionDelta.x, mousePositionDelta.y);

  if (!game->lock_camera) {
    // Make the cursor stay in the middle
    SetMousePosition(GetScreenWidth()/2, GetScreenHeight()/2);
  }

  const float speed = game->editor_open ? 120.0f : 25.0f;

  if (!game->editor_open) {
    // Apply gravity
  }

  // Handle collisions
  {
    physics->friction = 0.001f;
    physics->velocity.x += (sinf(CAMERA.angle.x) * direction[MOVE_BACK] -
                            sinf(CAMERA.angle.x) * direction[MOVE_FRONT] -
                            cosf(CAMERA.angle.x) * direction[MOVE_LEFT] +
                            cosf(CAMERA.angle.x) * direction[MOVE_RIGHT]) *
                           speed * GetFrameTime();

    physics->velocity.z += (cosf(CAMERA.angle.x) * direction[MOVE_BACK] -
                            cosf(CAMERA.angle.x) * direction[MOVE_FRONT] +
                            sinf(CAMERA.angle.x) * direction[MOVE_LEFT] -
                            sinf(CAMERA.angle.x) * direction[MOVE_RIGHT]) *
                           speed * GetFrameTime();
  }

  bool isMoving = false; // Required for swinging

  for (int i = 0; i < 6; i++)
    if (direction[i]) {
      isMoving = true;
      break;
    }

  // Camera orientation calculation
  CAMERA.angle.x += (mousePositionDelta.x * -CAMERA_MOUSE_MOVE_SENSITIVITY);
  CAMERA.angle.y += (mousePositionDelta.y * -CAMERA_MOUSE_MOVE_SENSITIVITY);

  // Angle clamp
  if (CAMERA.angle.y > CAMERA_FIRST_PERSON_MIN_CLAMP * DEG2RAD)
    CAMERA.angle.y = CAMERA_FIRST_PERSON_MIN_CLAMP * DEG2RAD;
  else if (CAMERA.angle.y < CAMERA_FIRST_PERSON_MAX_CLAMP * DEG2RAD)
    CAMERA.angle.y = CAMERA_FIRST_PERSON_MAX_CLAMP * DEG2RAD;

  // Recalculate camera target considering translation and rotation
  Matrix m_translation = MatrixTranslate(
      0, 0, (CAMERA.targetDistance / CAMERA_FREE_PANNING_DIVIDER));
  Matrix m_rotation = MatrixRotateXYZ(
      (Vector3){PI * 2 - CAMERA.angle.y, PI * 2 - CAMERA.angle.x, 0});
  Matrix m_transform = MatrixMultiply(m_translation, m_rotation);

  if (game->lock_camera) {
    return;
  }

  camera->position = Vector3Add(transform->translation, (Vector3){0, 0.75f, 0});

  camera->target.x = camera->position.x - m_transform.m12;
  camera->target.y = camera->position.y - m_transform.m13;
  camera->target.z = camera->position.z - m_transform.m14;

  if (isMoving)
    swingCounter++;

  camera->up.x = sinf(swingCounter /
                      (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) /
                 CAMERA_FIRST_PERSON_WAVING_DIVIDER;
  camera->up.z = -sinf(swingCounter /
                       (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) /
                 CAMERA_FIRST_PERSON_WAVING_DIVIDER;

  // Handle exits
  for (int i = 0; i < game->map->num_exits; i++) {
    Exit exit = game->map->exits[i];

    if (Vector3Distance(exit.position, transform->translation) < 0.5) {
      // Load the region from the exits

      reset_region_to_zero(game->map, game);
    }
  }
}

void draw_player_gui(Game *game, Region *map) {
  //
  const float timer = (float)GetTime();

  DrawText("W/A/S/D -> move", 10, 100, 20, WHITE);
  DrawText("Left click -> throw orange", 10, 130, 20, WHITE);
  DrawText("Right click -> throw pineapple bomb", 10, 160, 20, WHITE);
  DrawText("Escape -> release mouse", 10, 190, 20, WHITE);

  const int sw = GetScreenWidth();
  const int sh = GetScreenHeight();
  const int size = 128;
  const int border = 4;

  // Cross
  Color color = (Color){255, 255, 255, 200};
  DrawLine(sw / 2 - 10, sh / 2, sw / 2 + 10, sh / 2, color);
  DrawLine(sw / 2, sh / 2 - 10, sw / 2, sh / 2 + 10, color);

  // Pause
  int id = 0;

  // static bool do_exit_modal = false;
  if (do_btn(5, 5, 50, 50, "| |")) {
    game->scene = SCENE_MAIN_MENU;
  }
}
