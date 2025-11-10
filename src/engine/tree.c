#include "engine.h"

#include <stdlib.h>

void engine_attach_node(Node* parent, Node* child) {
    engine_detach_node(child);
    child->parent = parent;
    for (int i = 0; i < parent->children_size; i++) {
        if (parent->children[i] == NULL) {
            parent->children[i] = child;
            return;
        }
    }
    if (parent->children_size == parent->children_capacity) {
        parent->children_capacity *= 2;
        if (parent->children_capacity == 0) parent->children_capacity = 4;
        parent->children = realloc(parent->children, sizeof(Node*) * parent->children_capacity);
    }
    parent->children[parent->children_size++] = child;
}

void engine_detach_node(Node* child) {
    if (!child->parent) return;
    for (int i = 0; i < child->parent->children_size; i++) {
        if (child->parent->children[i] == child) {
            child->parent->children[i] = NULL;
            break;
        }
    }
    child->parent = NULL;
}

static void engine_delete_node_internal(Node* node) {
    for (int i = 0; i < node->children_size; i++) {
        if (!node->children[i]) continue;
        engine_delete_node_internal(node->children[i]);
    }
    free(node->children);
    free(node);
}

void engine_delete_node(Node* node) {
    engine_detach_node(node);
    engine_delete_node_internal(node);
}
