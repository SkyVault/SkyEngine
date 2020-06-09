#ifndef HARP_SCRIPTING_H
#define HARP_SCRIPTING_H

#include "game.h"
#include "janet.h"

typedef struct {
    JanetFunction* func;
} Script;

Script create_script(Game* game, const char* code);

#endif  // HARP_SCRIPTING_H