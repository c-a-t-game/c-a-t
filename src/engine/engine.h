#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <math.h>

#include "io/graphics.h"
#include "object_storage.h"

#define CHUNK_SIZE 16
#define OBJECT_STORAGE_SIZE 256

typedef union {
    int as_int[OBJECT_STORAGE_SIZE / sizeof(int)];
    int as_flt[OBJECT_STORAGE_SIZE / sizeof(float)];
    int as_ptr[OBJECT_STORAGE_SIZE / sizeof(void*)];
} ObjectStorage;

typedef enum {
#define NODE(type, ...) NodeType_##type,
#include "nodes.h"
#undef NODE
} NodeType;

typedef enum {
    Direction_None,
    Direction_Up,
    Direction_Left,
    Direction_Down,
    Direction_Right,
} Direction;

typedef struct Node Node;
struct Node {
    NodeType type;
    int children_size, children_capacity;
    Node** children;
    Node* parent;
    int size;
};

#define _(type, name, parser) typeof(type) name;
#define NODE(type, ...) typedef struct { Node node; __VA_ARGS__ } type##Node;
#include "nodes.h"
#undef NODE
#undef _

#define engine_new_node(t) ({ \
    t##Node* node = calloc(sizeof(t##Node), 1); \
    node->node.type = NodeType_##t; \
    node->node.size = sizeof(t##Node); \
    node; \
})

void engine_attach_node(Node* parent, Node* child);
void engine_detach_node(Node* child);
void engine_delete_node(Node* node);
Node* engine_deep_copy(Node* node);

uint8_t* engine_tile(TilemapNode* node, int x, int y);

void engine_update(LevelRootNode* node);
void engine_render(LevelRootNode* node, float width, float height);

#endif
