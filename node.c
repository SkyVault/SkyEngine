#include "node.h"

Node* create_node() {
    Node* node = malloc(sizeof(Node));

    node->transform = (Transform){.translation = Vector3Zero(),
                                  .rotation = QuaternionIdentity(),
                                  .scale = Vector3One()};

    node->parent = NULL;
    node->next = NULL;
    node->child = NULL;

    return node;
}

Node* create_node_from_mesh(Mesh mesh) {
    Node* node = create_node();
    node->model = LoadModelFromMesh(mesh);
    return node;
}

Node* create_node_from_mesh_with_transform(Mesh mesh, Transform transform) {
    Node* node = create_node();
    node->model = LoadModelFromMesh(mesh);
    node->transform = transform;
    return node;
}

void add_child_node(Node* parent, Node* child) {
    parent->child = child;
    child->parent = parent;
}

Transform get_transform_from_parent(Node* self) {}