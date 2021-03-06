#ifndef BENIS_MODELS_H
#define BENIS_MODELS_H

#include <stdlib.h>

#define RESOLUTION_WIDTH (480)
#define RESOLUTION_HEIGHT (RESOLUTION_WIDTH * (9.0 / 16.0))
#define RESOLUTION_ASPECT                                                      \
  (((float)RESOLUTION_HEIGHT) / ((float)RESOLUTION_WIDTH))

#include "assets.h"
#include "billboard.h"
#include "ecs.h"
#include "gameworld_types.h"
#include "node.h"
#include "prop.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#define MAX_NUMBER_OF_DRAWABLES (1000)
#define MAX_NUMBER_OF_3D_GUI_DRAWABLES (256)

#define FCOLOR(r, g, b, a)                                                     \
  (Color) {                                                                    \
    (unsigned char)((r)*255), (unsigned char)((g)*255),                        \
        (unsigned char)((b)*255), (unsigned char)((a)*255)                     \
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
  Drawable gui_3d_drawables[MAX_NUMBER_OF_3D_GUI_DRAWABLES];

  int num_opaque_drawables;
  int num_transparent_drawables;
  int num_gui_3d_drawables;

  RenderTexture2D render_texture;
} GfxState;

GfxState *create_gfx_state();

Transform transform_identity();
Transform transform_translation(Vector3 v);

void update_billboard(EcsWorld *ecs, EntId ent);
void draw_billboard_ent(GfxState *gfx, Camera *camera, EcsWorld *ecs,
                        EntId ent);

void update_models(EcsWorld *ecs, EntId ent); // Animations?
void draw_models(GfxState *gfx, EcsWorld *ecs, EntId ent);
void draw_model(GfxState *gfx, Model *model, Transform transform,
                Color diffuse);

void draw_gui_model(GfxState *gfx, Model *model, Rectangle where,
                    Vector3 rotation, Color diffuse);

void draw_billboard(GfxState *gfx, Vector3 position, Texture texture,
                    Rectangle region, float scale);

void draw_prop(GfxState *gfx, Assets* assets, Prop prop);

void begin_rendering(GfxState *gfx);
void end_rendering(GfxState *gfx);
void draw_final_texture_to_screen(GfxState *gfx);

void draw_root_node(GfxState *gfx, Node *node);

void draw_model_2d(GfxState *gfx, Model model, Rectangle region);

void flush_graphics(GfxState *gfx, Camera *camera);
void flush_gui_3d_graphics(GfxState *gfx);

#endif // BENIS_MODELS_H
