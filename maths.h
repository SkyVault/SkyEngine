#ifndef VAULT_MATHS_H
#define VAULT_MATHS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "raymath.h"

typedef union {
    float n[2];
    struct {
        float x;
        float y;
    };
    struct {
        float width;
        float height;
    };
} V2;

typedef union {
    float n[3];
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float width;
        float height;
        float depth;
    };
} V3;

typedef union {
    float n[4];
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float width;
        float height;
        float depth;
        float unit;
    };
} V4;

typedef union {
    int n[2];
    struct {
        int x;
        int y;
    };
    struct {
        int width;
        int height;
    };
} iV2;

V2 V2_Zero();
V2 V2_One();
V2 nV2(float x, float y);

V3 V3_Zero();
V3 V3_One();
V3 V3_Up();
V3 nV3(float x, float y, float z);

iV2 iV2_Zero();
iV2 iV2_One();
iV2 inV2(int x, int y);

V2 v2_add_v2(V2 a, V2 b);
V2 v2_sub_v2(V2 a, V2 b);
V2 v2_div_v2(V2 a, V2 b);
V2 v2_mul_v2(V2 a, V2 b);

V2 v2_add_f(V2 a, float b);
V2 v2_sub_f(V2 a, float b);
V2 v2_div_f(V2 a, float b);
V2 v2_mul_f(V2 a, float b);

static Transform add_transforms(Transform a, Transform b) {
    Transform result = {0};
    result.translation = Vector3Add(a.translation, b.translation);
    result.rotation = QuaternionMultiply(a.rotation, b.rotation);
    result.scale = Vector3Multiply(a.scale, b.scale);
    return result;
}

double fade(double t);
double lerp(double t, double a, double b);
double grad(int hash, double x, double y, double z);
double noise(double x, double y, double z);

void load_permutation();

float rand_f();

float lerp_p(float a, float b, float p);
float lerp_t(float a, float b, float t);

#endif  // VAULT_MATHS_H
