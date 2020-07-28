#include "node.h"

Node *create_node() {
  Node *node = malloc(sizeof(Node));

  node->type = NODE_TYPE_EMPTY;

  for (size_t i = 0; i < sizeof(node->name); i++)
      node->name[i] = '\0';

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
  assert(tree->type == NODE_TYPE_EMPTY && tree->child != NULL);

  if (to_delete->parent) {

    if (to_delete->child) {
      // Needs re-parent
    } else { 
    }

    // Find the node in the list so that we
    // can  know the prev

    Node* it = to_delete->parent->child;
    
    if (it == to_delete) { 
      to_delete->parent->child = it->next;
      delete_node(it); 
    }

    Node* prev = it;
    while (it != to_delete && it != NULL) { 
      prev = it;
      it = it->next;
    }

    prev->next = it->next;
    delete_node(it); 
  } 

  return false;
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
  if (parent->type == NODE_TYPE_EMPTY) {
    if (parent->child != NULL)
      node_prepend(parent->child, new);
    else
      parent->child = new;
    return;
  }

  if (parent->child != NULL) {
    node_prepend(parent->child, new);
  } else {
    Node *tmp = parent->next;
    parent->next = new;
    parent->next->next = tmp;
    new->parent = parent->parent;
  }
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

void pretty_print_node(Node* node) {
  // Print it as a s-expression
}
