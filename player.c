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
        dz);  // Camera angle in plane XZ (0 aligned with Z, move positive CCW)
    CAMERA.angle.y = atan2f(
        dy, sqrtf(dx * dx + dz * dz));  // Camera angle in plane XY (0 aligned
                                        // with X, move positive CW)

    CAMERA.playerEyesPosition = camera.position.y;

    // Lock cursor for first person and third person cameras
    if ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON))
        DisableCursor();
    else
        EnableCursor();

    CAMERA.mode = mode;
}

void throw_orange(EcsWorld* ecs, Vector3 pos, float angle, float yaw,
                  Assets* ass) {
    EntId bullet_id = create_ent(ecs);
    EntStruct* bullet = get_ent(ecs, bullet_id);

    pos.x += cosf(angle);
    pos.z += sinf(angle);
    pos.y -= 0.5f;

    add_comp(ecs, bullet, Transform, .translation = pos);
    add_comp_obj(ecs, bullet, Physics, create_physics());
    add_comp(ecs, bullet, Actor, .type = ACTOR_TOSSED_ORANGE, .state = IDLE);

    add_comp(ecs, bullet, Billboard, .texture = ass->textures[TEX_ORANGE],
             .material = (Material){0}, .scale = 1.0);

    add_comp(ecs, bullet, TimedDestroy, .time_left = 4.0f, .done = false);
    add_comp(ecs, bullet, PlayerHit, .damage = 1.0f);

    Physics* physics = get_comp(ecs, bullet, Physics);
    physics->velocity.x = cosf(angle) * 800.0f * GetFrameTime();
    physics->velocity.z = sinf(angle) * 800.0f * GetFrameTime();
    physics->velocity.y = 15.0f * yaw + 900.0f * GetFrameTime();
    physics->friction = 0.5f;
    physics->gravity_scale = 0.8f;
    physics->bounce_factor = 0.8f;
}

void throw_pineapple(EcsWorld* ecs, Vector3 pos, float angle, float yaw,
                     Assets* ass) {
    EntId bullet_id = create_ent(ecs);
    EntStruct* bullet = get_ent(ecs, bullet_id);

    pos.x += cosf(angle);
    pos.z += sinf(angle);
    pos.y -= 0.5f;

    add_comp(ecs, bullet, Transform, .translation = pos);
    add_comp_obj(ecs, bullet, Physics, create_physics());
    add_comp(ecs, bullet, Actor, .type = ACTOR_PINEAPPLE_BOMB, .state = IDLE);

    add_comp(ecs, bullet, Billboard, .texture = ass->textures[TEX_PINEAPPLE],
             .material = (Material){0}, .scale = 1.8);

    add_comp(ecs, bullet, TimedDestroy, .time_left = 4.0f, .done = false);
    add_comp(ecs, bullet, PlayerHit, .damage = 1.0f);

    Physics* physics = get_comp(ecs, bullet, Physics);
    physics->velocity.x = cosf(angle) * 600.0f * GetFrameTime();
    physics->velocity.z = sinf(angle) * 600.0f * GetFrameTime();
    physics->velocity.y = 15.0f * yaw + 980.0f * GetFrameTime();
    physics->friction = 0.5f;
    physics->gravity_scale = 1.0f;
    physics->bounce_factor = 0.8f;
}

void update_player(EcsWorld* ecs, Assets* ass, Game* game, EntId id) {
    EntStruct* self = get_ent(ecs, id);

    if (!has_comp(ecs, self, Player) || !has_comp(ecs, self, Transform) ||
        !has_comp(ecs, self, Physics))
        return;

    Camera* camera = game->camera;

    Transform* transform = get_comp(ecs, self, Transform);
    Physics* physics = get_comp(ecs, self, Physics);
    Player* player = get_comp(ecs, self, Player);

    const float angle = PI * 2 - CAMERA.angle.x - PI / 2;

    // Shoot
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        throw_orange(ecs, transform->translation, angle, CAMERA.angle.y, ass);
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        throw_pineapple(ecs, transform->translation, angle, CAMERA.angle.y,
                        ass);
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->lock_camera = !game->lock_camera;
    }

    static int first = 0;

    if (!first) {
        set_camera_mode(*camera, CAMERA_FIRST_PERSON);
        first = 1;
    }

    static int swingCounter = 0;  // Used for 1st person swinging movement
    static Vector2 previousMousePosition = {0.0f, 0.0f};

    // Mouse movement detection
    Vector2 mousePositionDelta = {0.0f, 0.0f};
    Vector2 mousePosition = GetMousePosition();

    bool direction[6] = {IsKeyDown(CAMERA.moveControl[MOVE_FRONT]),
                         IsKeyDown(CAMERA.moveControl[MOVE_BACK]),
                         IsKeyDown(CAMERA.moveControl[MOVE_RIGHT]),
                         IsKeyDown(CAMERA.moveControl[MOVE_LEFT]),
                         IsKeyDown(CAMERA.moveControl[MOVE_UP]),
                         IsKeyDown(CAMERA.moveControl[MOVE_DOWN])};

    mousePositionDelta.x = mousePosition.x - previousMousePosition.x;
    mousePositionDelta.y = mousePosition.y - previousMousePosition.y;

    previousMousePosition = mousePosition;

    // Handle collisions
    {
        physics->friction = 0.001f;
        physics->velocity.x += (sinf(CAMERA.angle.x) * direction[MOVE_BACK] -
                                sinf(CAMERA.angle.x) * direction[MOVE_FRONT] -
                                cosf(CAMERA.angle.x) * direction[MOVE_LEFT] +
                                cosf(CAMERA.angle.x) * direction[MOVE_RIGHT]) /
                               PLAYER_MOVEMENT_SENSITIVITY;

        physics->velocity.z += (cosf(CAMERA.angle.x) * direction[MOVE_BACK] -
                                cosf(CAMERA.angle.x) * direction[MOVE_FRONT] +
                                sinf(CAMERA.angle.x) * direction[MOVE_LEFT] -
                                sinf(CAMERA.angle.x) * direction[MOVE_RIGHT]) /
                               PLAYER_MOVEMENT_SENSITIVITY;
    }

    bool isMoving = false;  // Required for swinging

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

    camera->position = transform->translation;

    camera->target.x = camera->position.x - m_transform.m12;
    camera->target.y = camera->position.y - m_transform.m13;
    camera->target.z = camera->position.z - m_transform.m14;

    if (isMoving) swingCounter++;

    camera->up.x = sinf(swingCounter /
                        (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) /
                   CAMERA_FIRST_PERSON_WAVING_DIVIDER;
    camera->up.z = -sinf(swingCounter /
                         (CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER * 2)) /
                   CAMERA_FIRST_PERSON_WAVING_DIVIDER;
}

void draw_player_gui(Game* game, Map* map) {
    //
    Texture2D holding = game->assets->textures[TEX_PINEAPPLE];

    const float timer = GetTime();

    DrawText("W/A/S/D to move", 10, 100, 20, RAYWHITE);
    DrawText("Left click -> throw orange", 10, 130, 20, RAYWHITE);
    DrawText("Right click -> throw pineapple bomb", 10, 160, 20, RAYWHITE);

    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    const int size = 128;
    const int border = 4;

    // Cross
    Color color = (Color){255, 255, 255, 200};
    DrawLine(sw / 2 - 10, sh / 2, sw / 2 + 10, sh / 2, color);
    DrawLine(sw / 2, sh / 2 - 10, sw / 2, sh / 2 + 10, color);

    // DrawRectangle(0, (sh - size), size, size, RAYWHITE);
    // DrawRectangle(border, (sh - size) + border, size - border * 2,
    //               size - border * 2, BLACK);

    // Pause
    int id = 0;

    if (DoBtn(id, 5, 5, 50, 50, "| |")) {
    }

    // id = 100;
    // static char buffer[512];
    // DoTextInput(id++, buffer, 512, 10, 200, 300, 50);
}
