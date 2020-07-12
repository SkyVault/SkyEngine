#ifndef SKY_WORLD_INTERACTION_H
#define SKY_WORLD_INTERACTION_H

#include <raylib.h>

#include "gameworld.h"

struct NodeRayInfo {
  Node *node;
  RayHitInfo info;
};

Node *do_mouse_picking(Region *map, Camera *camera);

struct NodeRayInfo check_if_clicked(Ray ray, Node *node);

#endif // SKY_WORLD_INTERACTION_H
