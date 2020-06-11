#ifndef BENIS_MODELS_H
#define BENIS_MODELS_H

#include <stdlib.h>

#include "billboard.h"
#include "ecs.h"
#include "game.h"
#include "map_type.h"
#include "node.h"
#include "prop.h"
#include "raylib.h"
#include "raymath.h"

#define MAX_NUMBER_OF_DRAWABLES (1000)

#define FCOLOR(r, g, b, a)                                     \
    (Color) {                                                  \
        (unsigned char)((r)*255), (unsigned char)((g)*255),    \
            (unsigned char)((b)*255), (unsigned char)((a)*255) \
    }

#define VEC3_TO_COLOR(v) FCOLOR(v.x, v.y, v.z, 1.0f)
#define VEC4_TO_COLOR(v) FCOLOR((v).x, (v).y, (v).z, (v).w)

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
    Drawable opaque_drawables[MAX_NUMBER_OF_DRAWABLES];
    Drawable transparent_drawables[MAX_NUMBER_OF_DRAWABLES];

    int num_opaque_drawables;
    int num_transparent_drawables;
} GfxState;

GfxState* create_gfx_state();

void update_billboard(EcsWorld* ecs, EntId ent);
void draw_billboard_ent(GfxState* gfx, Camera* camera, EcsWorld* ecs,
                        EntId ent);

void update_models(EcsWorld* ecs, EntId ent);  // Animations?
void draw_models(GfxState* gfx, EcsWorld* ecs, EntId ent);
void draw_model(GfxState* gfx, Model* model, Transform transform,
                Color diffuse);

void draw_billboard(GfxState* gfx, Vector3 position, Texture texture,
                    Rectangle region, float scale);

void draw_prop(GfxState* gfx, Game* game, Prop prop);

void draw_node_tree(Node* node);

void flush_graphics(GfxState* gfx, Camera* camera);

#endif  // BENIS_MODELS_H
