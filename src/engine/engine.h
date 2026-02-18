#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "io/graphics.h"

typedef enum {
#define NODE(type, ...) NodeType_##type,
#include "assets/headers/nodes.h"
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
#include "assets/headers/nodes.h"
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
void* engine_property(EntityNode* node, const char* name);
EntityNode* engine_find_entity(LevelRootNode* level, const char* name);
EntityNode* engine_find_entity_on_tilemap(TilemapNode* tilemap, const char* name);

void engine_update(LevelRootNode* node, float delta_time);
void engine_render(LevelRootNode* node, float width, float height);

#endif
