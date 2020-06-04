#ifndef BENIS_NODE_H
#define BENIS_NODE_H

#include "maths.h"
#include "raylib.h"
#include "raymath.h"

typedef struct Node {
    Model model;
    Transform transform;

    struct Node* parent;
    struct Node* next;
    struct Node* child;
} Node;

Node* create_node();
Node* create_node_from_mesh(Mesh mesh);
Node* create_node_from_mesh_with_transform(Mesh mesh, Transform transform);

void add_child_node(Node* parent, Node* child);

Transform get_transform(Node* self);

#endif  // BENIS_NODE_H