#ifndef BENIS_NODE_H
#define BENIS_NODE_H

#include <assert.h>
#include <inttypes.h>

#include "billboard.h"
#include "maths.h"
#include "raylib.h"
#include "raymath.h"

enum {
  NODE_TYPE_EMPTY,
  NODE_TYPE_MODEL,
  NODE_TYPE_BILLBOARD,
};

enum {
  NODE_FLAG_NONE    = 1 << 0,
  NODE_FLAG_SOLID   = 1 << 1,
};

typedef struct Node {
  int type;
  
  union {
    struct {
      Model model;
      char name[256];
    };
    Billboard billboard;
  };

  Transform transform;

  struct Node *parent;
  struct Node *next;
  struct Node *child;

  uint32_t flags; 
} Node;

Node *create_node();
Node *create_node_from_mesh(Mesh mesh);
Node *create_node_from_mesh_with_transform(Mesh mesh, Transform transform);

Node *create_node_from_model(Model model, const char *name);
Node *create_node_from_model_with_transform(Model model, const char *name,
                                            Transform transform);

void node_prepend(Node *parent, Node *new);

bool delete_node_from_tree(Node *tree, Node *to_delete);
void delete_node_tree(Node *node);

inline Node *create_empty() { return create_node(); }

void add_child_node(Node *parent, Node *child);

Transform get_transform_from_node(Node *self);

void pretty_print_node(Node* node);

#endif // BENIS_NODE_H
