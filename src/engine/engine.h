#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <math.h>

#include "io/graphics.h"
#include "gpc.h"

#define CHUNK_SIZE 16

typedef enum {
    NodeType_LevelRoot,
    NodeType_TileChunk,
    NodeType_Tilemap,
    NodeType_Tileset,
    NodeType_Tile,
    NodeType_Entity,
    NodeType_CustomRender,
    NodeType_TileTexture,
    NodeType_TileEntityCollision,
    NodeType_EntityUpdate,
    NodeType_EntityTexture,
    NodeType_EntityCollision,
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
};

typedef struct {
    Node node;
    float cam_x, cam_y;
} LevelRootNode;

typedef struct {
    Node node;
    int x, y;
    uint8_t tile[CHUNK_SIZE][CHUNK_SIZE];
} TileChunkNode;

typedef struct {
    Node node;
    gpc_polygon collision;
    float scale_x, scale_y;
    float scroll_offset_x, scroll_offset_y;
    float scroll_speed_x, scroll_speed_y;
} TilemapNode;

typedef struct {
    Node node;
    Texture* tileset;
    int tile_width, tile_height;
    int tiles_per_row;
} TilesetNode;

typedef struct {
    Node node;
    gpc_polygon collision;
} TileNode;

typedef struct {
    Node node;
    float pos_x, pos_y;
    float vel_x, vel_y;
    float width, height;
} EntityNode;

typedef struct {
    Node node;
    void(*func)();
} CustomRenderNode;

typedef struct {
    Node node;
    int(*func)(TilemapNode* tilemap, TileNode* tile, int x, int y);
} TileTextureNode;

typedef struct {
    void(*func)(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction);
} TileEntityCollisionNode;

typedef struct {
    Node node;
    void(*func)(EntityNode* entity, TilemapNode* tilemap);
} EntityUpdateNode;

typedef struct {
    Node node;
    Texture*(*func)(EntityNode* entity, TilemapNode* tilemap, float* srcx, float* srcy, float* srcw, float* srch, float* w, float* h);
} EntityTextureNode;

typedef struct {
    void(*func)(EntityNode* entity, TilemapNode* tilemap, TileNode* tile, int x, int y, Direction direction);
} EntityCollisionNode;

#define engine_new_node(t) ({ \
    t##Node* node = calloc(sizeof(t##Node), 1); \
    node->node.type = NodeType_##t; \
    node; \
})

void engine_attach_node(Node* parent, Node* child);
void engine_detach_node(Node* child);
void engine_delete_node(Node* node);

void engine_compute_collision(TilemapNode* node);
void engine_set_tile(TilemapNode* node, int x, int y, uint8_t tile_index);
uint8_t engine_get_tile(TilemapNode* node, int x, int y);

void engine_update(LevelRootNode* node);
void engine_render(LevelRootNode* node, float width, float height);

void* loader_lvl(uint8_t* bytes, int size);

#endif