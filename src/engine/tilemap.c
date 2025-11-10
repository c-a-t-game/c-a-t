#include "engine.h"

#include <stdlib.h>

#include "gpc.h"

void engine_compute_collision(TilemapNode* node) {
    gpc_free_polygon(&node->collision);
    TilesetNode* tileset = NULL;
    for (int i = 0; i < node->node.children_size && !tileset; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type == NodeType_Tileset) tileset = (TilesetNode*)node->node.children[i];
    }
    if (!tileset) return;
    for (int i = 0; i < node->node.children_size; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_TileChunk) continue;
        TileChunkNode* chunk = (TileChunkNode*)node->node.children[i];
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                TileNode* tile = (TileNode*)tileset->node.children[chunk->tile[x][y]];
                gpc_polygon temp;
                gpc_polygon_clip(GPC_UNION, &node->collision, &tile->collision, &temp);
                gpc_free_polygon(&node->collision);
                node->collision = temp;
            }
        }
    }
}

static uint8_t* engine_tile(TilemapNode* node, int x, int y) {
    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;
    TileChunkNode* chunk = NULL;
    for (int i = 0; i < node->node.children_size && !chunk; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_TileChunk) continue;
        TileChunkNode* c = (TileChunkNode*)node->node.children[i];
        if (c->x != chunk_x || c->y != chunk_y) continue;
        chunk = c;
    }
    if (!chunk) {
        chunk = engine_new_node(TileChunk);
        chunk->x = chunk_x;
        chunk->y = chunk_y;
        engine_attach_node(&node->node, &chunk->node);
    }
    return &chunk->tile[x % CHUNK_SIZE][y % CHUNK_SIZE];
}

void engine_set_tile(TilemapNode* node, int x, int y, uint8_t tile_index) {
    *engine_tile(node, x, y) = tile_index;
}

uint8_t engine_get_tile(TilemapNode* node, int x, int y) {
    return *engine_tile(node, x, y);
}