#include "world_interaction.h"

Node *do_mouse_picking(Region *map, Camera *camera) {
  if (map->scene_root != NULL) {
    Vector2 mpos = GetMousePosition();
    Ray ray = GetMouseRay(mpos, *camera);
    return check_if_clicked(ray, map->scene_root);
  }
}

Node *check_if_clicked(Ray ray, Node *node) {
  if (node == NULL)
    return node;

  if (node->next) {
    Node *next = check_if_clicked(ray, node->next);
    if (next != NULL)
      return next;
  } 

  if (node->child) {
    Node *child = check_if_clicked(ray, node->child);
    if (child != NULL)
      return child;
  } 

  if (node->type == NODE_TYPE_MODEL) {
    BoundingBox box = MeshBoundingBox(node->model.meshes[0]);

    Vector3 pos = get_transform_from_node(node).translation;
    box.min = Vector3Add(box.min, pos);
    box.max = Vector3Add(box.max, pos);

    if (CheckCollisionRayBox(ray, box)) {
      if (GetCollisionRayModel(ray, node->model).hit) {
        return node;
      }
    }
  }

  return NULL;
}

