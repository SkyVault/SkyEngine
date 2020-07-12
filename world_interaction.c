#include "world_interaction.h"

Node *do_mouse_picking(Region *map, Camera *camera) {
  if (map->scene_root != NULL) {
    Vector2 mpos = GetMousePosition();
    Ray ray = GetMouseRay(mpos, *camera);

    Node *node = check_if_clicked(ray, map->scene_root).node;
    if (node == NULL) {
      printf("Nothing clicked! %d\n", rand());
    }
    return node;
  }
}

struct NodeRayInfo check_if_clicked(Ray ray, Node *node) {

  // Algorithm
  // 3. If the childs ray is closer then the closest one found in step 1, then
  // return the child, else the other

  struct NodeRayInfo nearest = {.node = NULL, .info = {0}};

  // 1. Iterate all nodes in the branch and find the one that was clicked, and
  // the closest to the ray start

  Node *it = node;

  while (it != NULL) {
    // TODO(Dustin):
    // We should be able to select empties at some point
    if (it->type == NODE_TYPE_MODEL) {
      BoundingBox box = MeshBoundingBox(it->model.meshes[0]);
      Vector3 pos = get_transform_from_node(it).translation;
      box.min = Vector3Add(box.min, pos);
      box.max = Vector3Add(box.max, pos);

      if (CheckCollisionRayBox(ray, box)) {
        RayHitInfo info = GetCollisionRayModel(ray, it->model);

        if (info.hit) {
          printf("here!!%d\n", rand());
          if (nearest.node != NULL) {
            if (info.distance < nearest.info.distance)
              nearest = (struct NodeRayInfo){.node = it, .info = info};
          } else {
            nearest = (struct NodeRayInfo){.node = it, .info = info};
          }
        }
      }
    }

    // Check the children
    if (it->child != NULL) {
      struct NodeRayInfo child_info = check_if_clicked(ray, it->child);
      if (child_info.node != NULL) {
        if (nearest.node != NULL) {
          if (child_info.info.distance < nearest.info.distance) {
            nearest = child_info;
          }
        } else {
          nearest = child_info;
        }
      }
    }

    it = it->next;
  }

  return nearest;
}
