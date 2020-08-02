#ifndef SKY_WORLD_INTERACTION_H
#define SKY_WORLD_INTERACTION_H

#include <raylib.h>

#include "utils.h"
#include "node.h"
#include "gameworld_types.h"

struct NodeRayInfo {
  Node *node;
  RayHitInfo info;
  struct NodeRayInfo *next;
};

typedef struct NodeRayInfo NodeRayInfo;

void free_node_ray_info(NodeRayInfo *self);

NodeRayInfo* do_raycast(Region *map, Ray ray);  // Requires cleanup
NodeRayInfo* get_intersecting_nodes(Ray ray, Node *node);

NodeRayInfo do_mouse_picking(Region *map, Camera *camera); 
NodeRayInfo check_if_clicked(Ray ray, Node *node);

#endif // SKY_WORLD_INTERACTION_H
