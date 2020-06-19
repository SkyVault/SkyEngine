#include "node.h"

Node* create_node() {
    Node* node = malloc(sizeof(Node));

    node->type = NODE_TYPE_EMPTY;

    node->transform = (Transform){.translation = Vector3Zero(),
                                  .rotation = QuaternionIdentity(),
                                  .scale = Vector3One()};

    node->parent = NULL;
    node->next = NULL;
    node->child = NULL;

    return node;
}

void free_node(Node* node) {
    if (node->child) free_node(node->child);
    if (node->next) free_node(node->next);
    node->child = node->next = node->parent = NULL;
    free(node);
}

void destroy_node_tree(Node* node) { free_node(node); }

Node* create_node_from_mesh(Mesh mesh) {
    Node* node = create_node();
    node->model = LoadModelFromMesh(mesh);
    node->type = NODE_TYPE_MODEL;
    return node;
}

Node* create_node_from_mesh_with_transform(Mesh mesh, Transform transform) {
    Node* node = create_node();
    node->model = LoadModelFromMesh(mesh);
    node->transform = transform;
    node->type = NODE_TYPE_MODEL;
    return node;
}

Node* create_node_from_model(Model model, const char* name) {
    Node* node = create_node();
    node->model = model;
    node->type = NODE_TYPE_MODEL;
    sprintf(node->name, "%s", name);
    return node;
}

Node* create_node_from_model_with_transform(Model model, const char* name, Transform transform) {
    Node* node = create_node();
    node->model = model;
    node->transform = transform;
    node->type = NODE_TYPE_MODEL;
    sprintf(node->name, "%s", name);
    return node;
}

void add_child_node(Node* parent, Node* child) {
    parent->child = child;
    child->parent = parent;
}

Transform get_transform_from_node(Node* self) {
    Transform self_t = self->transform;
    if (self->parent == NULL) return self_t;
    Transform parent_t = get_transform_from_node(self->parent);
    return add_transforms(self_t, parent_t);
}
