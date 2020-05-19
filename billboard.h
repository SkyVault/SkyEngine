#ifndef BENIS_BILLBOARD_H
#define BENIS_BILLBOARD_H

#include "raylib.h"

typedef struct {
    Texture2D texture;
    Material material;

    float scale;
} Billboard;

#endif  // BENIS_BILLBOARD_H
