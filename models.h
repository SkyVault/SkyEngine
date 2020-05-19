#ifndef BENIS_MODELS_H
#define BENIS_MODELS_H

#include <stdlib.h>

#include "billboard.h"
#include "ecs.h"
#include "game.h"
#include "map_type.h"
#include "prop.h"
#include "raylib.h"
#include "raymath.h"

#define MAX_NUMBER_OF_DRAWABLES (1000)

enum DrawFlags {
    DrawFlag_Active = 1 << 0,
    DrawFlag_NumDrawFlags = 1 << 1,
};

enum DrawTypes {
    DrawType_Billboard,
    DrawType_Model,
    DrawType_NumDrawTypes,
};

typedef struct Drawable {
    int type;
    int flags;

    Transform transform;
    Color diffuse;

    union {
        struct {
            Billboard billboard;
            Rectangle region;
        };
        Model model;
    };
} Drawable;

typedef struct {
    Drawable drawables[MAX_NUMBER_OF_DRAWABLES];
    int num_drawables;
} GfxState;

GfxState* create_gfx_state();

void update_billboard(EcsWorld* ecs, EntId ent);
void draw_billboard(GfxState* gfx, Camera* camera, EcsWorld* ecs, EntId ent);

void update_models(EcsWorld* ecs, EntId ent);  // Animations?
void draw_models(GfxState* gfx, EcsWorld* ecs, EntId ent);

void draw_prop(GfxState* gfx, Game* game, Prop prop);

void flush_graphics(GfxState* gfx, Camera* camera);

#endif  // BENIS_MODELS_H
