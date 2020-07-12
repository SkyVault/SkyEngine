#include "node.h"

Node *create_node() {
  Node *node = malloc(sizeof(Node));

  node->type = NODE_TYPE_EMPTY;
  node->name[0] = '\0';

  node->transform = (Transform){.translation = Vector3Zero(),
                                .rotation = QuaternionIdentity(),
                                .scale = Vector3One()};

  node->parent = NULL;
  node->next = NULL;
  node->child = NULL;

  return node;
}

void delete_node(Node *node) { free(node); }

void delete_node_rec(Node *node) {
  if (node->child)
    delete_node_rec(node->child);
  if (node->next)
    delete_node_rec(node->next);
  free(node);
}

bool delete_branch_node_rec(Node *node) {}

bool delete_node_from_tree(Node *tree, Node *to_delete) {
  // Algorithm:
  // 1. Assert that the tree root node is an empty
  // 2. loop over the nexts, and if any match to_delete, delete it
  //    2.1. for each next node, recursively call delete_branch_node_rec on each
  //    child, 2.2. if

  assert(tree->type == NODE_TYPE_EMPTY && tree->child != NULL);

  // bool deleted = false;

  // Node *it = tree->child;

  // while (!deleted) {
  //}
}

void delete_node_tree(Node *node) { delete_node_rec(node); }

Node *create_node_from_mesh(Mesh mesh) {
  Node *node = create_node();
  node->model = LoadModelFromMesh(mesh);
  node->type = NODE_TYPE_MODEL;
  return node;
}

Node *create_node_from_mesh_with_transform(Mesh mesh, Transform transform) {
  Node *node = create_node();
  node->model = LoadModelFromMesh(mesh);
  node->transform = transform;
  node->type = NODE_TYPE_MODEL;
  return node;
}

Node *create_node_from_model(Model model, const char *name) {
  Node *node = create_node();
  node->model = model;
  node->type = NODE_TYPE_MODEL;
  sprintf(node->name, "%s", name);
  return node;
}

Node *create_node_from_model_with_transform(Model model, const char *name,
                                            Transform transform) {
  Node *node = create_node();
  node->model = model;
  node->transform = transform;
  node->type = NODE_TYPE_MODEL;
  sprintf(node->name, "%s", name);
  return node;
}

void node_prepend(Node *parent, Node *new) {
  Node *tmp = parent->next;
  parent->next = new;
  parent->next->next = tmp;
}

void add_child_node(Node *parent, Node *child) {
  parent->child = child;
  child->parent = parent;
}

Transform get_transform_from_node(Node *self) {
  Transform self_t = self->transform;
  if (self->parent == NULL)
    return self_t;
  Transform parent_t = get_transform_from_node(self->parent);
  return add_transforms(self_t, parent_t);
}
