#include "renderer.h"

GfxState* create_gfx_state() {
    GfxState* gfx = malloc(sizeof(GfxState));
    for (int i = 0; i < MAX_NUMBER_OF_DRAWABLES; i++)
        gfx->drawables[i].flags = 0;
    gfx->num_drawables = 0;
    return gfx;
}

void update_billboard(EcsWorld* ecs, EntId ent) {}

void draw_billboard(GfxState* gfx, Camera* camera, EcsWorld* ecs, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);

    if (ent < 0 || self == NULL) return;

    if (!has_comp(ecs, self, Billboard) || !has_comp(ecs, self, Transform))
        return;

    if (gfx->num_drawables < MAX_NUMBER_OF_DRAWABLES) {
        Billboard b = *get_comp(ecs, self, Billboard);
        gfx->drawables[gfx->num_drawables++] = (Drawable){
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
    if (gfx->num_drawables < MAX_NUMBER_OF_DRAWABLES) {
        gfx->drawables[gfx->num_drawables++] = (Drawable){
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

void update_models(EcsWorld* ecs, EntId ent) {}

void draw_models(GfxState* gfx, EcsWorld* ecs, EntId ent) {
    EntStruct* self = get_ent(ecs, ent);

    if (ent < 0 || self == NULL) return;

    if (!has_comp(ecs, self, Model) || !has_comp(ecs, self, Transform)) return;

    if (gfx->num_drawables < MAX_NUMBER_OF_DRAWABLES) {
        Model* model = get_comp(ecs, self, Model);
        Color diffuse =
            model->materials[0].maps[MAP_DIFFUSE].color;  // paranoia?
        gfx->drawables[gfx->num_drawables++] = (Drawable){
            .type = DrawType_Model,
            .flags = DrawFlag_Active,
            .transform = *get_comp(ecs, self, Transform),
            .model = *model,
            .diffuse = diffuse,
        };
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

void flush_graphics(GfxState* gfx, Camera* camera) {
    scamera = camera;

    // qsort(gfx->drawables, gfx->num_drawables, sizeof(Drawable), sorting);
    // for (int i = MAX_NUMBER_OF_DRAWABLES - 1; i >= 0; i--) {
    for (int i = 0; i <= gfx->num_drawables; i++) {
        if (!(gfx->drawables[i].flags & DrawFlag_Active)) continue;

        Drawable* d = &gfx->drawables[i];

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

            fogFactor =
                fogFactor < 0 ? 0 : (fogFactor > 1.0f ? 1.0f : fogFactor);

            f.x = (f.x + (f.x * fogFactor)) - 1.0f;
            f.y = (f.y + (f.y * fogFactor)) - 1.0f;
            f.z = (f.z + (f.z * fogFactor)) - 1.0f;

            if (f.x < 0) f.x = 0;
            if (f.y < 0) f.y = 0;
            if (f.z < 0) f.z = 0;

            if (f.x > 1) f.x = 1;
            if (f.y > 1) f.y = 1;
            if (f.z > 1) f.z = 1;

            d->diffuse = (Color){f.x * 255, f.y * 255, f.z * 255, 255};

            DrawBillboardRec(*camera, d->billboard.texture, d->region, pos,
                             d->billboard.scale, d->diffuse);

            // DrawSphere(pos, 0.1f, RED);

        } else if (d->type == DrawType_Model) {
            d->model.transform = m;
            DrawModel(d->model, (Vector3){0, 0, 0}, 1, d->diffuse);
        }

         gfx->drawables[i] = (Drawable){0};
    }

    gfx->num_drawables = 0;
}
