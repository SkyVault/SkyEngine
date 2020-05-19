#ifndef BENIS_PHYSICS_TYPE_H
#define BENIS_PHYSICS_TYPE_H

#include "raylib.h"

typedef struct {
    Vector3 velocity;
    float friction;

    float gravity_scale;
    float bounce_factor;

    bool grounded;
} Physics;

Physics create_physics();

#endif//BENIS_PHYSICS_TYPE_H
