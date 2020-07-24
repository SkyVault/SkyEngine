#ifndef SKY_WORLD_INTERACTION_H
#define SKY_WORLD_INTERACTION_H

#include <raylib.h>

#include "gameworld.h"

struct NodeRayInfo {
  Node *node;
  RayHitInfo info;
};

typedef struct NodeRayInfo NodeRayInfo;

NodeRayInfo do_mouse_picking(Region *map, Camera *camera); 
NodeRayInfo check_if_clicked(Ray ray, Node *node);

#endif // SKY_WORLD_INTERACTION_H
