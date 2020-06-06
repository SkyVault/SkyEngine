#ifndef BENIS_NODE_H
#define BENIS_NODE_H

#include "billboard.h"
#include "maths.h"
#include "raylib.h"
#include "raymath.h"

enum {
    NODE_TYPE_EMPTY,
    NODE_TYPE_MODEL,
    NODE_TYPE_BILLBOARD,
};

typedef struct Node {
    int type;

    union {
        Model model;
        Billboard billboard;
    };

    Transform transform;

    struct Node* parent;
    struct Node* next;
    struct Node* child;
} Node;

Node* create_node();
Node* create_node_from_mesh(Mesh mesh);
Node* create_node_from_mesh_with_transform(Mesh mesh, Transform transform);

inline Node* create_empty() { return create_node(); }

void add_child_node(Node* parent, Node* child);

Transform get_transform(Node* self);

#endif  // BENIS_NODE_H