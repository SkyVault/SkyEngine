#ifndef BENIS_PLAYER_H
#define BENIS_PLAYER_H

#include <math.h>

#include "assets.h"
#include "components.h"
#include "ecs.h"
#include "entity_type.h"
#include "gameworld_types.h"
#include "gui.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"
#include "renderer.h"

#define PLAYER_START_JUMP_STRENGTH 3200.0f
#define PLAYER_START_GRAVITY_SCALE 0.8f

typedef struct {
    int n;

    float rotation;

    Texture2D holding;
} Player;

typedef enum {
    MOVE_FRONT = 0,
    MOVE_BACK,
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_UP,
    MOVE_DOWN
} CameraMove;

typedef struct {
    int mode;                  // Current camera mode
    float targetDistance;      // Camera distance from position to target
    float playerEyesPosition;  // Default player eyes position from ground (in
                               // meters)
    Vector2 angle;             // Camera angle in plane XZ
    int moveControl[6];
    int smoothZoomControl;  // raylib: KEY_LEFT_CONTROL
    int altControl;         // raylib: KEY_LEFT_ALT
    int panControl;         // raylib: MOUSE_MIDDLE_BUTTON
} CameraData;

static CameraData CAMERA = {.mode = 0,
                            .targetDistance = 0,
                            .playerEyesPosition = ACTOR_HEIGHT,
                            .angle = {0},
                            .moveControl = {KEY_W, 'S', 'D', 'A', 'E', 'Q'},
                            .smoothZoomControl = 341,
                            .altControl = 342,
                            .panControl = 2};

void update_player(EcsWorld* ecs, Assets* ass, Game* camera, EntId self);
void draw_player_gui(Game* game, Region* map);

#endif  // BENIS_PLAYER_H
