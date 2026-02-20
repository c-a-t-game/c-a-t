#include "engine.h"

#include <stdlib.h>
#include <string.h>

static Node deleted_nodes;

static void engine_mark_deleted(Node* node) {
    for (int i = 0; i < node->children_size; i++) {
        if (!node->children[i]) continue;
        engine_mark_deleted(node->children[i]);
    }
    engine_attach_node(&deleted_nodes, node);
}

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

void engine_delete_node(Node* node) {
    engine_mark_deleted(node);
}

Node* engine_deep_copy(Node* node) {
    Node* copy = malloc(node->size);
    memcpy(copy, node, node->size);
    copy->children = malloc(sizeof(Node*) * copy->children_capacity);
    copy->parent = NULL;
    for (int i = 0; i < copy->children_size; i++) {
        if (node->children[i] == NULL) {
            copy->children[i] = NULL;
            continue;
        }
        Node* child = engine_deep_copy(node->children[i]);
        child->parent = copy;
        copy->children[i] = child;
    }
    return copy;
}

void engine_cleanup() {
    for (int i = 0; i < deleted_nodes.children_size; i++) {
        Node* node = deleted_nodes.children[i];
        free(node->children);
        free(node);
    }
    deleted_nodes.children_size = 0;
}
