#ifndef VAULT_COMPONENTS_H
#define VAULT_COMPONENTS_H

#include "ecs.h"
#include "maths.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"

typedef struct {
    V2 pos;
    V2 size;
} Body;

int contains(Body* a, Body* b);
V2 center(Body* b);

typedef struct {
    float time_left;
} TimedDestroy;

enum Facing {
    Facing_X,
    Facing_Z,
};

enum DoorState { Door_Open, Door_Closed, };

typedef struct {
    int facing;
    int state;

    float timer;
} Door;

void update_timed_destroy(EcsWorld* world, EntId id);
void update_doors(EcsWorld* world, EntId id);

#endif//VAULT_COMPONENTS_H
