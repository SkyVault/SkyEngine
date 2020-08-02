#include "world_interaction.h"

NodeRayInfo* new_node_ray_info() {
    NodeRayInfo *result = talloc(sizeof(NodeRayInfo));
    result->info = (RayHitInfo){0};
    result->node = NULL;
    result->next = NULL;
    return result;
}

NodeRayInfo *do_raycast(Region *map, Ray ray) {
  if (map->scene_root != NULL) {
      return get_intersecting_nodes(ray, map->scene_root);
  }
  
  return NULL;
}

NodeRayInfo do_mouse_picking(Region *map, Camera *camera) {
  if (map->scene_root != NULL) {
    Vector2 mpos = GetMousePosition();
    Ray ray = GetMouseRay(mpos, *camera);

    NodeRayInfo info = check_if_clicked(ray, map->scene_root); 
    return info;
  }
  
  return (NodeRayInfo){.node = NULL, .info = {0}};
}

NodeRayInfo* get_intersecting_nodes(Ray ray, Node *node) {
    NodeRayInfo *head = NULL;
    NodeRayInfo *result = NULL;

    Node *it = node;
    while (it != NULL) {
        if (it->type == NODE_TYPE_MODEL) {
            BoundingBox box = MeshBoundingBox(it->model.meshes[0]);
            Vector3 pos = get_transform_from_node(it).translation;
            box.min = Vector3Add(box.min, pos);
            box.max = Vector3Add(box.max, pos);

            if (CheckCollisionRayBox(ray, box)) {
                RayHitInfo info = GetCollisionRayModel(ray, it->model);

                if (!head) {
                    head = new_node_ray_info();
                    result = head;
                }

                if (info.hit) {
                    head->next = new_node_ray_info();
                    head->next->info = info;
                    head->next->node = it;
                    head = head->next;
                }
            }
        }

        if (it->child != NULL) {
            NodeRayInfo *child_info = get_intersecting_nodes(ray, it->child);
            if (child_info != NULL) {
                child_info = child_info->next;
                if (child_info && child_info->node != NULL) {
                    if (!head) {
                        head = new_node_ray_info();
                        result = head;
                    }
                    head->next = child_info;
                    head = head->next;
                }
            }
        }

        it = it->next;
    }

    return result;
}

NodeRayInfo check_if_clicked(Ray ray, Node *node) {
 NodeRayInfo nearest = {.node = NULL, .next = NULL, .info = {0}};

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
          if (nearest.node != NULL) {
            if (info.distance < nearest.info.distance)
                nearest = (NodeRayInfo){.node = it, .next = NULL, .info = info};
          } else {
              nearest = (NodeRayInfo){.node = it, .next = NULL, .info = info};
          }
        }
      }
    }

    // Check the children
    if (it->child != NULL) {
      NodeRayInfo child_info = check_if_clicked(ray, it->child);
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
