#include "engine.h"

#include <stdlib.h>
#include <string.h>

static int compare_string(const void* a, const void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

void engine_set_tile(TilemapNode* node, int x, int y, uint8_t tile) {
    if (x < node->start_x || y < node->start_y || x >= node->end_x || y >= node->end_y || !node->tiles) {
        int growth_left = 0, growth_right = 0, growth_top = 0, growth_bottom = 0;
        if (x < node->start_x) growth_left  = (node->start_x - x + 16) / 16 * 16;
        if (y < node->start_y) growth_top   = (node->start_y - y + 16) / 16 * 16;
        if (x >= node->end_x)  growth_right  = (x -  node->end_x + 16) / 16 * 16;
        if (y >= node->end_y)  growth_bottom = (y -  node->end_y + 16) / 16 * 16;
        int old_start_x = node->start_x, old_start_y = node->start_y, old_end_x = node->end_x, old_end_y = node->end_y;
        node->start_x -= growth_left;
        node->start_y -= growth_top;
        node->end_x += growth_right;
        node->end_y += growth_bottom;
        int pitch = node->end_x - node->start_x;
        int old_pitch = old_end_x - old_start_x;
        size_t size = (node->end_x - node->start_x) * (node->end_y - node->start_y);
        uint8_t* new_tiles = malloc(size);
        memset(new_tiles, 0, size);
        if (node->tiles) for (int y = old_start_y; y < old_end_y; y++)
            for (int x = old_start_x; x < old_end_x; x++)
                new_tiles[(y - node->start_y) * pitch + (x - node->start_x)] = node->tiles[(y - old_start_y) * old_pitch + (x - old_start_x)];
        free(node->tiles);
        node->tiles = new_tiles;
    }
    int pitch = node->end_x - node->start_x;
    node->tiles[(y - node->start_y) * pitch + (x - node->start_x)] = tile;
}

uint8_t engine_get_tile(TilemapNode* node, int x, int y) {
    if (x < node->start_x || y < node->start_y || x >= node->end_x || y >= node->end_y || !node->tiles) return node->oob_tile_provider(node, x, y);
    int pitch = node->end_x - node->start_x;
    return node->tiles[(y - node->start_y) * pitch + (x - node->start_x)];
}

void* engine_property(EntityNode* node, const char* name) {
    if (node->data.entries) {
        void* ptr = bsearch(&name, node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string);
        if (ptr) return &((typeof(*node->data.entries)*)ptr)->value;
    }
    if (node->data.count == node->data.capacity) {
        if (node->data.capacity == 0) node->data.capacity = 4;
        else node->data.capacity *= 2;
        node->data.entries = realloc(node->data.entries, node->data.capacity * sizeof(*node->data.entries));
        memset(node->data.entries + node->data.count, 0, (node->data.capacity - node->data.count) * sizeof(*node->data.entries));
    }
    node->data.entries[node->data.count++].key = (char*)name;
    qsort(node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string);
    return &((typeof(*node->data.entries)*)bsearch(&name, node->data.entries, node->data.count, sizeof(*node->data.entries), compare_string))->value;
}

EntityNode* engine_find_entity(LevelRootNode* node, const char* name) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_Tilemap) continue;
        EntityNode* entity = engine_find_entity_on_tilemap((TilemapNode*)node->node.children[i], name);
        if (entity) return entity;
    }
    return NULL;
}

EntityNode* engine_find_entity_on_tilemap(TilemapNode* node, const char* name) {
    for (int i = 0; i < node->node.children_size; i++) {
        if (!node->node.children[i]) continue;
        if (node->node.children[i]->type != NodeType_Entity) continue;
        EntityNode* entity = (EntityNode*)node->node.children[i];
        if (entity->name && strcmp(entity->name, name) == 0) return entity;
    }
    return NULL;
}
