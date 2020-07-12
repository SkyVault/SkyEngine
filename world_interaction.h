#ifndef SKY_WORLD_INTERACTION_H
#define SKY_WORLD_INTERACTION_H

#include <raylib.h>

#include "gameworld.h"

Node *do_mouse_picking(Region *map, Camera *camera); 
Node *check_if_clicked(Ray ray, Node *node); 

#endif//SKY_WORLD_INTERACTION_H
