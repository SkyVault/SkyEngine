#include "renderer.h"

GfxState* create_gfx_state() {
    GfxState* gfx = malloc(sizeof(GfxState));
    for (int i = 0; i < MAX_NUMBER_OF_DRAWABLES; i++) {
        gfx->opaque_drawables[i].flags = 0;
        gfx->transparent_drawables[i].flags = 0;
    }
    gfx->num_opaque_drawables = 0;
    gfx->num_transparent_drawables = 0;

    gfx->render_texture = rlLoadRenderTexture(
        RESOLUTION_WIDTH, RESOLUTION_HEIGHT, UNCOMPRESSED_R8G8B8A8, 24, true);
    // LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    return gfx;
}

void update_billboard(EcsWorld* ecs, EntId ent) {}

void draw_billboard_ent(GfxState* gfx, Camera* camera, EcsWorld* ecs,
                        EntId ent) {
    EntStruct* self = get_ent(ecs, ent);

    if (ent < 0 || self == NULL) return;

    if (!has_comp(ecs, self, Billboard) || !has_comp(ecs, self, Transform))
        return;

    if (gfx->num_transparent_drawables < MAX_NUMBER_OF_DRAWABLES) {
        Billboard b = *get_comp(ecs, self, Billboard);
        gfx->transparent_drawables[gfx->num_transparent_drawables++] =
            (Drawable){
                .type = DrawType_Billboard,
                .flags = DrawFlag_Active,
                .transform = *get_comp(ecs, self, Transform),
                .billboard = b,
                .diffuse = WHITE,
                .region = (Rectangle){0, 0, b.texture.width, b.texture.height},
            };
    }
}

void draw_prop(GfxState* gfx, Game* game, Prop prop) {
    if (gfx->num_transparent_drawables < MAX_NUMBER_OF_DRAWABLES) {
        gfx->transparent_drawables[gfx->num_transparent_drawables++] =
            (Drawable){
                .type = DrawType_Billboard,
                .flags = DrawFlag_Active,
                .region = prop.region,
                .transform =
                    (Transform){
                        .translation = prop.position,
                        .rotation = QuaternionIdentity(),
                        .scale = (Vector3){1, 1, 1},
                    },
                .billboard =
                    (Billboard){.texture = game->assets->textures[TEX_PROPS],
                                .material = {0},
                                .scale = prop.scale},
                .diffuse = WHITE,
            };
    }
}

void draw_billboard(GfxState* gfx, Vector3 position, Texture texture,
                    Rectangle region, float scale) {
    if (gfx->num_transparent_drawables < MAX_NUMBER_OF_DRAWABLES) {
        gfx->transparent_drawables[gfx->num_transparent_drawables++] =
            (Drawable){
                .type = DrawType_Billboard,
                .flags = DrawFlag_Active,
                .region = region,
                .transform =
                    (Transform){
                        .translation = position,
                        .rotation = QuaternionIdentity(),
                        .scale = (Vector3){1, 1, 1},
                    },
                .billboard =
                    (Billboard){
                        .texture = texture, .material = {0}, .scale = scale},
                .diffuse = WHITE,
            };
    }
}

void update_models(EcsWorld* ecs, EntId ent) {}

void add_opaque_drawable(GfxState* gfx, Drawable drawable) {}

void add_transparent_drawable(GfxState* gfx, Drawable drawable) {}

void draw_node_tree(Node* node) {
    if (node->type == NODE_TYPE_MODEL) {
    }
}

void draw_model(GfxState* gfx, Model* model, Transform transform,
                Color diffuse) {
    if (diffuse.a < 255) {
        if (gfx->num_transparent_drawables < MAX_NUMBER_OF_DRAWABLES) {
            gfx->transparent_drawables[gfx->num_transparent_drawables++] =
                (Drawable){
                    .type = DrawType_Model,
                    .flags = DrawFlag_Active,
                    .transform = transform,
                    .model = *model,
                    .diffuse = diffuse,
                };
        }
    } else {
        if (gfx->num_opaque_drawables < MAX_NUMBER_OF_DRAWABLES) {
            gfx->opaque_drawables[gfx->num_opaque_drawables++] = (Drawable){
                .type = DrawType_Model,
                .flags = DrawFlag_Active,
                .transform = transform,
                .model = *model,
                .diffuse = diffuse,
            };
        }
    }
}

void draw_models(GfxState* gfx, EcsWorld* ecs, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);

    if (ent < 0 || self == NULL) return;

    if (!has_comp(ecs, self, Model) || !has_comp(ecs, self, Transform)) return;

    Model* model = get_comp(ecs, self, Model);
    Color diffuse = model->materials[0].maps[MAP_DIFFUSE].color;  // paranoia?

    if (diffuse.a < 255) {
        if (gfx->num_transparent_drawables < MAX_NUMBER_OF_DRAWABLES) {
            gfx->transparent_drawables[gfx->num_transparent_drawables++] =
                (Drawable){
                    .type = DrawType_Model,
                    .flags = DrawFlag_Active,
                    .transform = *get_comp(ecs, self, Transform),
                    .model = *model,
                    .diffuse = diffuse,
                };
        }
    } else {
        if (gfx->num_opaque_drawables < MAX_NUMBER_OF_DRAWABLES) {
            gfx->opaque_drawables[gfx->num_opaque_drawables++] = (Drawable){
                .type = DrawType_Model,
                .flags = DrawFlag_Active,
                .transform = *get_comp(ecs, self, Transform),
                .model = *model,
                .diffuse = diffuse,
            };
        }
    }
}

static Camera* scamera = NULL;

int sorting(const void* _a, const void* _b) {
    Drawable* a = (Drawable*)_a;
    Drawable* b = (Drawable*)_b;

    if (scamera == NULL) return 0;

    // Cache this?
    float ad = Vector3Distance(a->transform.translation, scamera->position);
    float bd = Vector3Distance(b->transform.translation, scamera->position);

    return (ad < bd) ? 1 : -1;
}

void render_drawable(Drawable* d, Camera* camera) {
    Vector3 pos = d->transform.translation;
    Vector3 scale = d->transform.scale;

    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixScale(scale.x, scale.y, scale.z));
    m = MatrixMultiply(m, QuaternionToMatrix(d->transform.rotation));
    m = MatrixMultiply(m, MatrixTranslate(pos.x, pos.y, pos.z));

    const Vector4 fogColor = (Vector4){0.f, 0.f, 0.f, 1.0f};
    const float fogDensity = 0.015;

    if (d->type == DrawType_Billboard) {
        Vector3 f = {d->diffuse.r / 255.f, d->diffuse.g / 255.f,
                     d->diffuse.b / 255.f};

        Vector2 sub = (Vector2){
            d->transform.translation.x,
            d->transform.translation.z,
        };

        float dist = Vector2Distance(
            sub, (Vector2){camera->position.x, camera->position.z});

        float fogFactor =
            1.0f / exp((dist * fogDensity * 8) * (dist * fogDensity));

        fogFactor = fogFactor < 0 ? 0 : (fogFactor > 1.0f ? 1.0f : fogFactor);

        f.x = (f.x + (f.x * fogFactor)) - 1.0f;
        f.y = (f.y + (f.y * fogFactor)) - 1.0f;
        f.z = (f.z + (f.z * fogFactor)) - 1.0f;

        if (f.x < 0) f.x = 0;
        if (f.y < 0) f.y = 0;
        if (f.z < 0) f.z = 0;

        if (f.x > 1) f.x = 1;
        if (f.y > 1) f.y = 1;
        if (f.z > 1) f.z = 1;

        d->diffuse = VEC3_TO_COLOR(f);

        DrawBillboardRec(*camera, d->billboard.texture, d->region, pos,
                         d->billboard.scale, d->diffuse);

    } else if (d->type == DrawType_Model) {
        d->model.transform = m;
        DrawModel(d->model, Vector3Zero(), 1, d->diffuse);
    }
}

void begin_rendering(GfxState* gfx) {
    BeginTextureMode(gfx->render_texture);
    ClearBackground((Color){0, 0, 0, 100});
}

void end_rendering(GfxState* gfx) { EndTextureMode(); }

void draw_final_texture_to_screen(GfxState* gfx) {
    DrawTexturePro(gfx->render_texture.texture,
                   (Rectangle){0, 0, gfx->render_texture.texture.width,
                               -gfx->render_texture.texture.height},
                   (Rectangle){0, 0, GetScreenWidth(),
                               GetScreenWidth() * RESOLUTION_ASPECT},
                   (Vector2){0, 0}, 0.0f, WHITE);
}

void draw_root_node(GfxState* gfx, Node* node) {
    if (!node) return;

    Transform transform = get_transform_from_node(node);

    DrawGizmo(transform.translation);

    switch (node->type) {
        case NODE_TYPE_MODEL: {
            draw_model(gfx, &node->model, transform, WHITE);
            break;
        }

        case NODE_TYPE_BILLBOARD: {
            draw_billboard(gfx, transform.translation, node->billboard.texture,
                           (Rectangle){0, 0, node->billboard.texture.width,
                                       node->billboard.texture.height},
                           1.0f);
            break;
        }

        default: {
        }
    }

    if (node->next) {
        draw_root_node(gfx, node->next);
    }

    if (node->child) {
        draw_root_node(gfx, node->child);
    }
}

void flush_graphics(GfxState* gfx, Camera* camera) {
    scamera = camera;

    // for (int i = MAX_NUMBER_OF_DRAWABLES - 1; i >= 0; i--) {
    for (int i = 0; i <= gfx->num_opaque_drawables; i++) {
        if (!(gfx->opaque_drawables[i].flags & DrawFlag_Active)) continue;
        Drawable* d = &gfx->opaque_drawables[i];
        render_drawable(d, camera);
        gfx->opaque_drawables[i] = (Drawable){0};
    }

    gfx->num_opaque_drawables = 0;

    qsort(gfx->transparent_drawables, gfx->num_transparent_drawables,
          sizeof(Drawable), sorting);

    for (int i = 0; i <= gfx->num_transparent_drawables; i++) {
        if (!(gfx->transparent_drawables[i].flags & DrawFlag_Active)) continue;
        Drawable* d = &gfx->transparent_drawables[i];
        render_drawable(d, camera);
        gfx->transparent_drawables[i] = (Drawable){0};
    }
    gfx->num_transparent_drawables = 0;
}