#ifndef BENIS_MAP_H
#define BENIS_MAP_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assets.h"
#include "components.h"
#include "ecs.h"
#include "game.h"
#include "map_def.h"
#include "physics_type.h"
#include "raylib.h"
#include "raymath.h"

struct LvlData {
    int w, h;
    char *d;

    struct LvlData *next_layer;
};

static const struct LvlData map_data[100] = {[0] = {.w = 18,
                                                    .h = 10,
                                                    .d = "########## #######"
                                                         "#........###.....#"
                                                         "#...#....|.......#"
                                                         "#........###.....#"
                                                         "#..##-##.#.###-###"
                                                         "#........|...#...#"
                                                         "#........#####...#"
                                                         "#....#...#.......#"
                                                         "#........#.......#"
                                                         "############.#####",
                                                    .next_layer = NULL}};

Map *load_map_from_file(const char *path, Game *game);

Map *load_map(int map, Game *game);

void update_map(Map *map, Game *game);
void draw_map(Map *map, Game *game);

#endif  // BENIS_MAP_H
