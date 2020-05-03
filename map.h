#ifndef BENIS_MAP_H
#define BENIS_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "game.h"
#include "components.h"
#include "ecs.h"
#include "assets.h"
#include "physics_type.h"
#include "raylib.h"
#include "raymath.h"
#include "map_def.h"

struct LvlData
{
    int w, h;
    char *d;

    struct LvlData *next_layer;
};

static const struct LvlData map_data[100] = {
    [0] = {
        .w = 18,
        .h = 10,
        .d = "########## #######"
             "#        ###     #"
             "#   #    |       #"
             "#        ###     #"
             "#  ##-## # ###-###"
             "#        |   #   #"
             "#        #####   #"
             "#    #   #       #"
             "#        #       #"
             "############ #####",
        .next_layer = NULL}};

Map *load_map_from_file(const char *path, Game *game);

Map *load_map(int map, Game *game);

void update_map(Map *map, Game *game);
void draw_map(Map *map, Game *game);

#endif //BENIS_MAP_H
